#include "demodulate_top.hpp"
#include <stdlib.h>
#include <math.h>
#include <tuple>
#include <iostream>
#include <sstream>

int crc_test();

#include "audio_data.hpp"

const uint8_t packet[] = {
    0x82, 0xa0, 0xa6, 0x64, 0x64, 0x68, 0x60, 0x96, 0x88, 0x6c, 0x8c, 0xac, 0xa0, 0x64, 0x9c, 0x6c,
    0x8a, 0xb0, 0x40, 0x40, 0xe2, 0xae, 0x92, 0x88, 0x8a, 0x62, 0x40, 0x61, 0x03, 0xf0, 0x3e, 0x31,
    0x35, 0x32, 0x33, 0x34, 0x33, 0x7a, 0x5b, 0x32, 0x32, 0x34, 0x5d, 0x2a, 0x57, 0x65, 0x20, 0x6b,
    0x6e, 0x6f, 0x77, 0x20, 0x6d, 0x6f, 0x73, 0x74, 0x20, 0x6f, 0x66, 0x20, 0x79, 0x6f, 0x75, 0x72,
    0x20, 0x66, 0x61, 0x75, 0x6c, 0x74, 0x73, 0x21, 0x21, 0x21, 0x0d, 0xd5, 0xb3,
};

int hdlc_test();

int main()
{
    idata_type tmp_in;
	odata_type tmp_out;

    bool failed = hdlc_test();
	int count = 0;
	tmp_out.last = 0;
	lock_type locked;
	cancel_type cancel = 0;

	int good_count = 0;

	if (failed) {
		std::cout << "HDLC test failed" << std::endl;
	}

	std::ostringstream spacket;
	istream_type tmp_ins;
	ostream_type tmp_outs;

	for (int i = 0; i != 60; ++i) {
		for (int j = 0; j != 26400; ++j) {
			tmp_in.data = audio[i * 26400 + j];
			tmp_in.last = 0;
			tmp_in.strb = 1;
			tmp_in.keep = 1;

			tmp_ins << tmp_in;
		}

		demodulate_top(tmp_ins, cancel, tmp_outs, locked);

		while (!tmp_outs.empty()) {
			tmp_outs >> tmp_out;
			uint8_t tmp = tmp_out.data;
			spacket << char(tmp);
			if (tmp_out.last) {
				if (tmp & 1) {
					good_count += 1;
					std::cout << "DEMOD GOOD[" << int(tmp) << "]: " << spacket.str() << std::endl;
					auto t = spacket.str();
					if (good_count == 3) {
						for (size_t i = 0; i != sizeof(packet) && i != t.size(); ++i) {
							if (uint8_t(t[i]) != packet[i]) {
								std::cout << "FAILED due to packet mismatch" << std::endl;
								failed = true;
								break;
							}
						}
					}
				} else {
					std::cout << "DEMOD BAD[" << int(tmp) << "]: " << spacket.str() << std::endl;
				}
				spacket.str("");
			}
		}
	}

	for (int i = 0; i != 120; ++i)
	{
		tmp_in.data = 0;
		tmp_in.last = 0;
		tmp_ins << tmp_in;
	}

	tmp_in.data = 0;
	tmp_in.last = 1;
	tmp_ins << tmp_in;
	demodulate_top(tmp_ins, cancel, tmp_outs, locked);

	if (good_count != 51) {
		std::cout << "FAILED due to good count mismatch -- " << good_count << " != 1" << std::endl;
		failed = 1;
	}

    if (crc_test()) {
    	failed = true;
    	std::cout << "FAILED due to CRC test failure" << std::endl;
    }

    cancel = 1;
    demodulate_top(tmp_ins, cancel, tmp_outs, locked);
    while (!tmp_outs.empty())
    	tmp_outs >> tmp_out;

	if (tmp_out.data != 2) {
		std::cout << "FAILED due to incorrect cancellation code: " << int(tmp_out.data) << std::endl;
		failed = 1;
	}
	if (tmp_out.last != 1) {
		std::cout << "FAILED due to incorrect last flag" << std::endl;
		failed = 1;
	}
	if (cancel != 0) {
		std::cout << "FAILED due to incorrect cancel flag" << std::endl;
		failed = 1;
	}

    return failed;
}

