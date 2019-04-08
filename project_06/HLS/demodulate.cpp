#include "demodulate.hpp"
#include "digital_pll.hpp"
#include "hdlc.hpp"
#include "nrzi.hpp"

#include "ap_shift_reg.h"

#include <iostream>

const ap_int<13> bpf_coeffs[] =
{    0,     0,     0,     0,     0,     0,     1,     3,     5,     8,     8,     5,
    -2,   -13,   -27,   -40,   -46,   -44,   -32,   -12,    11,    32,    44,    44,
    32,    14,     0,    -2,    13,    49,    97,   143,   170,   160,   104,     6,
  -118,  -244,  -340,  -381,  -352,  -258,  -120,    24,   138,   192,   173,    97,
     0,   -67,   -56,    62,   287,   575,   850,  1021,  1001,   737,   228,  -462,
 -1216, -1879, -2293, -2336, -1956, -1182,  -133,  1008,  2030,  2736,  2988,  2736,
  2030,  1008,  -133, -1182, -1956, -2336, -2293, -1879, -1216,  -462,   228,   737,
  1001,  1021,   850,   575,   287,    62,   -56,   -67,     0,    97,   173,   192,
   138,    24,  -120,  -258,  -352,  -381,  -340,  -244,  -118,     6,   104,   160,
   170,   143,    97,    49,    13,    -2,     0,    14,    32,    44,    44,    32,
    11,   -12,   -32,   -44,   -46,   -40,   -27,   -13,    -2,     5,     8,     8,
     5,     3,     1,     0,     0,     0,     0,     0,     0,
};

const ap_int<12> lpf_coeffs[] =
{
    0,    0,    0,    1,    3,    5,    8,   11,   14,   17,   20,   21,   20,   17,
   11,    2,   -9,  -25,  -44,  -66,  -91, -116, -142, -167, -188, -205, -215, -217,
 -209, -190, -156, -109,  -47,   30,  123,  230,  350,  481,  622,  769,  919, 1070,
 1217, 1358, 1488, 1605, 1704, 1785, 1844, 1880, 1893, 1880, 1844, 1785, 1704, 1605,
 1488, 1358, 1217, 1070,  919,  769,  622,  481,  350,  230,  123,   30,  -47, -109,
 -156, -190, -209, -217, -215, -205, -188, -167, -142, -116,  -91,  -66,  -44,  -25,
   -9,    2,   11,   17,   20,   21,   20,   17,   14,   11,    8,    5,    3,    1,
	0,    0,    0,
};

template <typename InOut, typename Filter, size_t N>
InOut fir_filter(InOut x, Filter (&coeff)[N])
{
    static InOut shift_reg[N];

    ap_int<31> accum = 0;
    filter_loop: for (int i = N-1 ; i != 0; i--)
    {
#pragma HLS UNROLL factor=20
        shift_reg[i] = shift_reg[i-1];
        accum += shift_reg[i] * coeff[i];
    }
    shift_reg[0] = x;
    accum += shift_reg[0] * coeff[0];

    return static_cast<InOut>(accum >> 15);
}

ap_shift_reg<bool, 12> delay_line;
DigitalPLL<> dpll(26400.0, 1200.0);

ap_uint<8> buffer[4096];
hdlc_decoder<sizeof(buffer)> hdlc(buffer, false);
nrzi afsk_nrzi;

/*
void write_byte(ostream_type& out2, ap_uint<8> data, ap_uint<1> last)
{
	odata_type output;

	output.data = data;
	output.dest = 0;
	output.id = 0;
	output.keep = 1;
	output.last = last;
	output.strb = 1;
	output.user = 0;
	out2 << output;
}
*/

void demod(ap_int<16> data, bool& bit, bool& sample_out, bool& locked_out)
{
	ap_int<16> bpfiltered = fir_filter(data, bpf_coeffs);
	ap_uint<1>comp = bpfiltered[15] == 0;
	ap_uint<1>delayed = delay_line.shift(comp);
	ap_uint<1> corr = comp ^ delayed;
	ap_int<2> corr_norm =  corr ? 1 : -1;
	ap_int<16> lpfiltered = fir_filter(corr_norm, lpf_coeffs);
	bit = !lpfiltered[15];
	dpll.pll(bit, sample_out, locked_out);
}

void demodulate6(istream_type& in, ostream_type& out, lock_type& lock, done_type done)
{
#pragma HLS INTERFACE axis port=in
#pragma HLS INTERFACE axis port=out
#pragma HLS INTERFACE ap_none port=lock name=lock_led
#pragma HLS INTERFACE s_axilite port=done

	static bool undone = false;

	idata_type input;
	odata_type output;

#ifndef __SYNTHESIS__
	std::cout << "input size = " << in.size() << std::endl;
	int count = 0;
#endif

	is_done: if (done && !undone) {
		undone = true;
		hdlc.start_search();
		lock = 0;
		output.data = 0;
		output.dest = 0;
		output.id = 0;
		output.keep = 1;
		output.last = 1;
		output.strb = 1;
		output.user = 0;
		out << output;
		// write_byte(out, 0, 1);
	} else {
		read_in: while (!in.empty()) {
			undone = false;
#ifndef __SYNTHESIS__
			++count;
#endif
			in >> input;

			bool bit;
			bool sample;
			bool locked;

			demod(input.data, bit, sample, locked);

			lock = locked;
#ifndef __SYNTHESIS__
			std::cout << (locked ? "+" : "-");
#endif

			should_sample: if (sample) {
#ifndef __SYNTHESIS__
				std::cout << "s" << std::endl;
#endif
				uint16_t length;
				bool valid;
				bool packet;
				const bool nzbit = afsk_nrzi.decode(bit);
				hdlc.decode_bit(nzbit, locked, length, packet, valid);
				if (packet) {
#ifndef __SYNTHESIS__
					std::cout << "packet!" << std::endl;
#endif
					const uint16_t eob = length - 1;
					write_packet_byte: for (uint16_t i = 0; i != length; ++i) {
						// write_byte(out, buffer[i], 0);
						output.data = buffer[i];
						output.dest = input.dest;
						output.id = input.id;
						output.keep = input.keep;
						output.last = i == eob ? 1 : 0;
						output.strb = 1;
						output.user = input.user;
						out << output;
					}
				}
			}
		}
	}
#ifndef __SYNTHESIS__
	std::cout << "processed = " << count << std::endl;
#endif
}
