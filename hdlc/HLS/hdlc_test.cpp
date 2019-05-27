#include "dpll_data.hpp"
#include "hdlc_top.hpp"

#include <string>
#include <iostream>

int main()
{
	std::string packet;
	int failed = 0;
	int count = 0;
	int good_count = 0;

	ostream_type output;
	istream_type input;
	odata_type odata;
	idata_type idata;

	for (size_t i = 0; i != sizeof(dpll_data); ++i) {
		idata.data = dpll_data[i];
		idata.last = i == (sizeof(dpll_data) - 1);
		input << idata;
		if (i % 264 == 263)
		{
			hdlc_top(output, input, 0);
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
