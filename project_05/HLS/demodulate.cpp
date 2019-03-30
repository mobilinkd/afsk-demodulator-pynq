#include "demodulate.hpp"
#include "digital_pll.hpp"

#include "ap_shift_reg.h"

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

    int32_t accum = 0;
    filter_loop: for (size_t i = N-1 ; i != 0; i--)
    {
#pragma HLS unroll factor=20
        shift_reg[i] = shift_reg[i-1];
        accum += shift_reg[i] * coeff[i];
    }

    shift_reg[0] = x;
    accum += shift_reg[0] * coeff[0];

    return static_cast<InOut>(accum >> 15);
}

ap_shift_reg<bool, 12> delay_line;
DigitalPLL<> dpll(26400, 1200);

void demodulate5(idata_type& input, odata_type& output)
{
#pragma HLS INTERFACE axis port=input
#pragma HLS INTERFACE axis port=output
#pragma HLS interface ap_ctrl_none port=return

	ap_int<16> bpfiltered, lpfiltered;
	ap_int<1> comp, delayed, comp2;
	ap_int<2> corr;

	bpfiltered = fir_filter(input.data, bpf_coeffs);
	comp = bpfiltered >= 0 ? 1 : 0;
	delayed = delay_line.shift(comp);
	corr = comp ^ delayed;
	corr <<= 1;
	corr -= 1;
	lpfiltered = fir_filter(corr, lpf_coeffs);
	comp2 = lpfiltered >= 0 ? 1 : 0;
	typename DigitalPLL<>::result_type result = dpll(comp2 != 0);

	ap_int<3> tmp = (std::get<0>(result) << 2) |
			(std::get<1>(result) << 1) | std::get<2>(result);
	output.data = tmp;
    output.dest = input.dest;
    output.id = input.id;
    output.keep = input.keep;
    output.last = input.last;
    output.strb = input.strb;
    output.user = input.user;
}
