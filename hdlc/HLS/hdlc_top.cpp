#include "hdlc_top.hpp"
#include "hdlc.hpp"
#include "nrzi.hpp"

void hdlc_top(
	ostream_type& output,
	input_type input, sample_type sample, lock_type pll_lock,
	cancel_type cancel)
{
#pragma HLS INTERFACE axis both port=output
#pragma HLS INTERFACE s_axilite port=input bundle=data
#pragma HLS INTERFACE s_axilite port=pll_lock bundle=data
#pragma HLS INTERFACE s_axilite port=sample bundle=data
#pragma HLS INTERFACE s_axilite port=cancel bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

	static nrzi nrzi_;

#pragma HLS PIPELINE

	static odata_type out;
	if (sample) {
		hdlc(out.data, out.keep, out.last, nrzi_.decode(input), pll_lock, cancel);
		out.strb = out.keep;
		output << out;
	}
}
