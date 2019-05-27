#include "dpll_data.hpp"
#include "hdlc_top.hpp"

#include <string>
#include <iostream>

void hdlc_top(
		ostream_type& output, input_type input, sample_type sample, lock_type pll_lock, cancel_type cancel);

int main()
{
	std::string packet;
	int failed = 0;
	int count = 0;
	int good_count = 0;

	ostream_type output;
	odata_type odata;

	for (size_t i = 0; i != sizeof(dpll_data); ++i) {
		ap_uint<3> data = dpll_data[i];
		ap_uint<1> bit = data[2];
		ap_uint<1> sample = data[1];
		ap_uint<1> lock = data[0];

		hdlc_top(output, bit, sample, lock, 0);
	}

	while (!output.empty())
	{
		output >> odata;
		if (odata.keep) {
			if (odata.last) {
				count += 1;
				if (odata.data & 1) {
					std::cout << "HDLC GOOD[" << int(odata.data) << "]: " << packet << std::endl;
					good_count += 1;
				} else {
					std::cout << "HDLC BAD[" << int(odata.data) << "]: " << packet << std::endl;
				}
				packet.clear();
			} else {
				packet += char(odata.data);
			}
		}
	}

	if (good_count != 4)
	{
		std::cout << "failed due to incorrect good_count: " << good_count << " (should be 4)" << std::endl;
		failed = 1;
	}

	std::cout << "HDLC packets = " << count << std::endl;

	return failed;
}
