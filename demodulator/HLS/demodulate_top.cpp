// Copyright 2019 Mobilinkd LLC <rob@mobilinkd.com>
// All rights reserved.

#include "demodulate_top.hpp"
#include "demodulate.hpp"

void demodulate_top(
	istream_type& input, cancel_type& cancel,
	ostream_type& output, lock_type& lock)
{
#pragma HLS INTERFACE axis off port=input
#pragma HLS INTERFACE s_axilite register port=cancel bundle=CTRL name=cancel
#pragma HLS INTERFACE axis off port=output
#pragma HLS INTERFACE ap_none register port=lock name=lock_led
#pragma HLS INTERFACE s_axilite port=return bundle=CTRL

	static odata_type odata;
	static idata_type idata;
	static lock_type lockv;

	odata.strb = 1;

	if (cancel) {
		mobilinkd::demodulate(
			odata.data, odata.keep, odata.last, lockv,
			ap_int<16>(0), cancel);
		cancel = 0;
		lock = lockv;
		output << odata;
	} else {
		while (!input.empty()) {
			input >> idata;
			mobilinkd::demodulate(
				odata.data, odata.keep, odata.last, lockv,
				idata.data, cancel);

			lock = lockv;
			if (odata.keep) {
				output << odata;
			}
		}
	}
}
