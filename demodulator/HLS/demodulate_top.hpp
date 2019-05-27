// Copyright 2019 Mobilinkd LLC <rob@mobilinkd.com>
// All rights reserved.
#ifndef MOBILINKD__DEMODULATE_TOP_HPP_
#define MOBILINKD__DEMODULATE_TOP_HPP_

#pragma once

#include "demodulate.hpp"

#include <ap_axi_sdata.h>
#include <hls_stream.h>
#include <stdint.h>

#define BPF_COEFF_LEN 141

typedef ap_axis<16,0,0,0> idata_type;
typedef hls::stream<idata_type> istream_type;
typedef ap_axiu<8,0,0,0> odata_type;
typedef hls::stream<odata_type> ostream_type;
typedef ap_int<1> lock_type;
typedef uint32_t cancel_type;

void demodulate_top(
		istream_type& input, cancel_type& cancel,
		ostream_type& output, lock_type& lock);

#endif // MOBILINKD__DEMODULATE_TOP_HPP_
