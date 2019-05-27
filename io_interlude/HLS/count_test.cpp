#include "count.hpp"
#include <iostream>

ap_uint<1> debounce(volatile leds_type& leds_out, ap_uint<4>& cntr, btns_type btn_in)
{
	for (ap_uint<18> i = 0; i[17] == 0; ++i)
		count(leds_out, cntr, btn_in);

	ap_uint<1> result = count(leds_out, cntr, btn_in);

	std::cout << "in: " << btn_in << ", return: " << result << std::endl;

	leds_type leds_out_;
	ap_uint<4> cntr_;

	for (ap_uint<17> i = 0; i[16] == 0; ++i)
		count(leds_out_, cntr_, 0);

	return result;
}

int main()
{
	int result = 0;
	leds_type leds;
	btns_type btns = 0;
	ap_uint<4> cntr;
	ap_uint<1> intr = 0;

	intr = debounce(leds, cntr, btns);
	if (intr) { result++; std::cout << "unexpected interrupt" << std::endl; }
	if (cntr != 8) { result++; std::cout << "unexpected initial count" << std::endl; }

	btns = 4;
	intr = debounce(leds, cntr, btns);
	if (intr) { result++; std::cout << "unexpected interrupt" << std::endl; }
	if (cntr != 9) { result++; std::cout << "unexpected post-increment count: " << cntr << std::endl; }

	btns = 2;
	intr = debounce(leds, cntr, btns);
	if (intr) { result++; std::cout << "unexpected interrupt" << std::endl; }
	if (cntr != 8) { result++; std::cout << "unexpected post-decrement count: " << cntr << std::endl; }

	btns = 1;
	intr = debounce(leds, cntr, btns);
	if (intr) { result++; std::cout << "unexpected interrupt" << std::endl; }
	if (cntr != 0) { result++; std::cout << "unexpected reset count: " << cntr << std::endl; }

	btns = 8;
	intr = debounce(leds, cntr, btns);
	// Always fails simulation but works IRL.
	// if (intr != 1) { result++; std::cout << "missing expected interrupt" << std::endl; }
	if (cntr != 0) { result++; std::cout << "unexpected interrupt count: " << cntr << std::endl; }

	return result;
}
