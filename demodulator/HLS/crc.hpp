// Copyright 2019 Mobilinkd LLC <rob@mobilinkd.com>
// All rights reserved.

#ifndef MOBILINKD__CRC_HPP_
#define MOBILINKD__CRC_HPP_

#include "ap_int.h"

struct crc16_ccitt {
	ap_uint<16> crc{0xFFFF};

	crc16_ccitt()
	{
#pragma HLS RESOURCE variable=crc core=FIFO_SRL
	}

	void compute(ap_uint<1> bit) {
#pragma HLS INLINE
#pragma HLS PIPELINE
		ap_uint<1> tmp = crc[15] ^ bit;

		compute_shift: crc <<= 1;

		crc[0] = tmp;
		crc[5] = crc[5] ^ tmp;
		crc[12] = crc[12] ^ tmp;
	}

	void reset() {
		crc = 0xFFFF;
	}

	ap_uint<16> value() const {
#pragma HLS INLINE
		ap_uint<16> value = crc;
		value.reverse();
		return value;
	}
};

#endif // MOBILINKD__CRC_HPP_
