#include "hdlc_top.hpp"
#include "hdlc.hpp"

void hdlc_top(
	ostream_type& output,
	input_type input, sample_type sample, lock_type pll_lock,
	cancel_type cancel)
{
#pragma HLS INTERFACE axis register both port=output
#pragma HLS INTERFACE s_axilite port=input bundle=data clock=hdlc_clk
#pragma HLS INTERFACE s_axilite port=pll_lock bundle=data clock=hdlc_clk
#pragma HLS INTERFACE s_axilite port=cancel bundle=control clock=hdlc_clk

#pragma HLS PIPELINE
	static odata_type out;
	if (sample) {
		hdlc(out.data, out.keep, out.last, input, sample, pll_lock, cancel);
		out.strb = out.keep;
		output << out;
	}
}
