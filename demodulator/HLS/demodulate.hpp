// Copyright 2019 Mobilinkd LLC <rob@mobilinkd.com>
// All rights reserved.
#ifndef MOBILINKD__DEMODULATE_HPP_
#define MOBILINKD__DEMODULATE_HPP_

#pragma once

#include <ap_int.h>

namespace mobilinkd {

typedef ap_int<16> idata_type;
typedef ap_uint<8> odata_type;
typedef ap_uint<1> valid_type;
typedef ap_uint<1> last_type;
typedef ap_int<1> lock_type;
typedef ap_uint<1> cancel_type;

void demodulate(
	odata_type& output, valid_type& valid, last_type& last, lock_type& lock,
	idata_type input, cancel_type cancel);

}

#endif // MOBILINKD__DEMODULATE_HPP_
