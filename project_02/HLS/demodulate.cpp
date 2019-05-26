// Copyright 2019 Mobilinkd LLC.

#include "demodulate.hpp"

const ap_int<13> bpf_coeffs[] =
{
	     1,     3,     5,     8,     8,     5,    -2,   -13,   -27,   -40,   -46,   -44,
	   -32,   -12,    11,    32,    44,    44,    32,    14,     0,    -2,    13,    49,
	    97,   143,   170,   160,   104,     6,  -118,  -244,  -340,  -381,  -352,  -258,
	  -120,    24,   138,   192,   173,    97,     0,   -67,   -56,    62,   287,   575,
	   850,  1021,  1001,   737,   228,  -462, -1216, -1879, -2293, -2336, -1956, -1182,
	  -133,  1008,  2030,  2736,  2988,  2736,  2030,  1008,  -133, -1182, -1956, -2336,
	 -2293, -1879, -1216,  -462,   228,   737,  1001,  1021,   850,   575,   287,    62,
	   -56,   -67,     0,    97,   173,   192,   138,    24,  -120,  -258,  -352,  -381,
	  -340,  -244,  -118,     6,   104,   160,   170,   143,    97,    49,    13,    -2,
	     0,    14,    32,    44,    44,    32,    11,   -12,   -32,   -44,   -46,   -40,
	   -27,   -13,    -2,     5,     8,     8,     5,     3,     1,
};


template <typename InOut, typename Filter, size_t N>
inline ap_int<16> fir_filter(InOut x, InOut (&shift_reg)[N], Filter (&coeff)[N], size_t shift = (InOut::width - 1))
{
#pragma HLS pipeline
    ap_int<36> accum{0};
    filter_loop: for (size_t i = N-1 ; i != 0; i--)
    {
        InOut tmp = shift_reg[i-1];
        shift_reg[i] = tmp;
        ap_int<InOut::width + Filter::width> tmp2 = tmp * coeff[i];
        accum += tmp2;
    }

    shift_reg[0] = x;
    accum += (x * coeff[0]);

    return static_cast<ap_int<16>>(accum >> shift);
}

void demodulate(stream_type& input, stream_type& output)
{
#pragma HLS INTERFACE axis port=input
#pragma HLS INTERFACE axis port=output
#pragma HLS interface ap_ctrl_none port=return

	iodata_type idata;
	iodata_type odata;

	static ap_int<16> bpf_shift_reg[BPF_COEFF_LEN];
#pragma HLS ARRAY_PARTITION variable=bpf_shift_reg complete dim=1

	demod_loop: while (!input.empty())
	{
		input >> idata;
		odata.data = fir_filter(idata.data, bpf_shift_reg, bpf_coeffs);
		odata.keep = idata.keep;
		odata.strb = idata.strb;
		odata.last = idata.last;
		output << odata;
	}
}
