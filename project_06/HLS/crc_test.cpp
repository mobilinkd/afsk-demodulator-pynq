#include "crc.hpp"
#include <cstdio>

const uint8_t packet[] = {
    0x82, 0xa0, 0xa6, 0x64, 0x64, 0x68, 0x60, 0x96, 0x88, 0x6c, 0x8c, 0xac, 0xa0, 0x64, 0x9c, 0x6c,
    0x8a, 0xb0, 0x40, 0x40, 0xe2, 0xae, 0x92, 0x88, 0x8a, 0x62, 0x40, 0x61, 0x03, 0xf0, 0x3e, 0x31,
    0x35, 0x32, 0x33, 0x34, 0x33, 0x7a, 0x5b, 0x32, 0x32, 0x34, 0x5d, 0x2a, 0x57, 0x65, 0x20, 0x6b,
    0x6e, 0x6f, 0x77, 0x20, 0x6d, 0x6f, 0x73, 0x74, 0x20, 0x6f, 0x66, 0x20, 0x79, 0x6f, 0x75, 0x72,
    0x20, 0x66, 0x61, 0x75, 0x6c, 0x74, 0x73, 0x21, 0x21, 0x21, 0x0d, 0xd5, 0xb3,
};

int crc_test()
{
	crc16_ccitt crc;

	bool failed = false;

	for (int i = 0; i != sizeof(packet); ++i) {
		uint8_t c = packet[i];
		for (int j = 0; j != 8; ++j) {
			ap_uint<1> bit = (c & 0x1 ? 1 : 0);
			c >>= 1;
			crc.compute(bit);
		}
	}
	std::cout << "crc = "<< std::hex << crc.value() << std::endl;
	failed = crc.value() != 0xf0b8;

	// verify reset() works.
	crc.reset();

	for (int i = 0; i != sizeof(packet); ++i) {
		uint8_t c = packet[i];
		for (int j = 0; j != 8; ++j) {
			ap_uint<1> bit = (c & 0x1 ? 1 : 0);
			c >>= 1;
			crc.compute(bit);
		}
	}
	failed |= crc.value() != 0xf0b8;
	std::cout << "crc = "<< std::hex << crc.value() << std::endl;
	std::cout << "crc = "<< std::hex << crc.value() << std::endl;

	return failed;
}
