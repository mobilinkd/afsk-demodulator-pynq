#include "count.hpp"

int main()
{
	int result = 0;
	leds_type leds;
	btns_type btns = 0;
	int cntr;
	ap_uint<1> intr = 0;

	intr = count(leds, cntr, btns);
	if (intr) result++;
	if (cntr != 8) result++;

	btns = 1;
	intr = count(leds, cntr, btns);
	if (intr) result++;
	if (cntr != 9) result++;

	btns = 2;
	intr = count(leds, cntr, btns);
	if (intr) result++;
	if (cntr != 8) result++;

	btns = 4;
	intr = count(leds, cntr, btns);
	if (intr) result++;
	if (cntr != 0) result++;

	btns = 8;
	intr = count(leds, cntr, btns);
	if (!intr) result++;
	if (cntr != 0) result++;
}
