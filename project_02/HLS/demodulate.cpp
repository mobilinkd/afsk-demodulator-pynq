#include "demodulate.hpp"

const int16_t bpf_coeffs[] =
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

template <typename InOut, typename Filter, size_t N>
InOut fir_filter(InOut x, Filter (&coeff)[N])
{
	static InOut shift_reg[N];

	int32_t accum = 0;
	filter_loop: for (size_t i = N-1 ; i != 0; i--)
	{
		shift_reg[i] = shift_reg[i-1];
		accum += shift_reg[i] * coeff[i];
	}

	shift_reg[0] = x;
	accum += shift_reg[0] * coeff[0];

	return static_cast<InOut>(accum >> 15);
}

void demodulate2(stream_type input[BLOCK_SIZE], stream_type output[BLOCK_SIZE])
{
#pragma HLS INTERFACE axis port=input
#pragma HLS INTERFACE axis port=output
#pragma HLS interface ap_ctrl_none port=return

	demod_loop: for (size_t i = 0; i != BLOCK_SIZE; ++i) {
		stream_type tmp = input[i];
		tmp.data = fir_filter(input[i].data, bpf_coeffs);
		output[i] = tmp;
	}
}
