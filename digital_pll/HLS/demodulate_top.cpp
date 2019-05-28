// Copyright 2019 Mobilinkd LLC <rob@mobilinkd.com>
// All rights reserved.

#include "demodulate_top.hpp"
#include "digital_pll.hpp"

#include "ap_shift_reg.h"

DigitalPLL<> dpll(26400, 1200);

void demodulate_top(istream_type& input, ostream_type& output, ap_int<1>& locked)
{
#pragma HLS INTERFACE axis port=input
#pragma HLS INTERFACE axis port=output
#pragma HLS INTERFACE s_axilite port=return bundle=CTRL
#pragma HLS INTERFACE ap_none port=locked name=locked

	static idata_type idata;
	static odata_type odata;

	odata.strb = 1;
	odata.keep = 1;

	while (!input.empty())
	{
		input >> idata;
		ap_uint<1> sample_out;
		ap_uint<1> locked_out;

		dpll.pll(idata.data, sample_out, locked_out);
		odata.data[2] = idata.data;
		odata.data[1] = sample_out;
		odata.data[0] = locked_out;
		odata.last = idata.last;
		locked = locked_out;
		output << odata;
	}
}
