#include "dpll_data.hpp"
#include "hdlc.hpp"
#include "nrzi.hpp"

#include <string>
#include <iostream>

int hdlc_test()
{
	std::string packet;
	int result = 1;
	int count = 0;

	nrzi decoder;

	for (size_t i = 0; i != sizeof(dpll_data); ++i) {
		ap_uint<3> data = dpll_data[i];
		ap_uint<1> bit = data[2];
		ap_uint<1> sample = data[1];
		ap_uint<1> lock = data[0];

		ap_uint<8> out;
		ap_uint<1> valid;
		ap_uint<1> last;

		if (sample) {
			hdlc(out, valid, last, decoder.decode(bit), lock, 0);
			if (valid) {
				if (last) {
					count += 1;
					if (out & 1) {
						std::cout << "HDLC GOOD[" << int(out) << "]: " << packet << std::endl;
						result = 0;
					} else {
						std::cout << "HDLC BAD[" << int(out) << "]: " << packet << std::endl;
					}
					packet.clear();
				} else {
					packet += char(out);
				}
			}
		}
	}

	std::cout << "HDLC packets = " << count << std::endl;

	return result;
}
