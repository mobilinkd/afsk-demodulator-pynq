#include "count.hpp"

ap_uint<1> count(volatile leds_type& leds_out, ap_uint<4>& cntr, btns_type btn_in)
{
#pragma HLS INTERFACE ap_none port=leds_out name=led
#pragma HLS INTERFACE s_axilite port=cntr
#pragma HLS INTERFACE ap_none port=btn_in name=btn
#pragma HLS INTERFACE ap_ctrl_hs port=return

	static ap_uint<4> counter = 8;
	static auto button = Button();
	static ap_uint<1> result = 0;

#pragma HLS PIPELINE
	leds_out = counter;

	btns_type buttons = button.state(btn_in);

	if (buttons[2] != 0) {
		// Verify count is not at max.
		if (counter != 0xf) counter += 1;
	}
	if (buttons[1] != 0) {
		// Verify count is not at min.
		if (counter != 0x0) counter -= 1;
	}
	if (buttons[0] != 0) {
		counter = 0;
	}

	cntr = counter;

	result = buttons[3];

	return result;
}
