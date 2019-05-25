// Copyright 2019 Mobilinkd LLC <rob@mobilinkd.com>
// All rights reserved.

#ifndef MOBILINKD__HDLC_DECODER_TOP_HPP_
#define MOBILINKD__HDLC_DECODER_TOP_HPP_

#include <ap_axi_sdata.h>
#include <hls_stream.h>
#include <ap_int.h>
#include <cstdint>
#include "crc.hpp"

typedef ap_axiu<8,1,1,1> odata_type;
typedef hls::stream<odata_type> ostream_type;
typedef ap_uint<1> input_type;
typedef ap_uint<1> sample_type;
typedef ap_uint<1> lock_type;
typedef ap_uint<1> cancel_type;

void hdlc_top(ostream_type& output, input_type input, sample_type sample, lock_type pll_lock, cancel_type cancel);

#endif // MOBILINKD__HDLC_DECODER_TOP_HPP_
