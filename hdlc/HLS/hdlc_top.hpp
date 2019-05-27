// Copyright 2019 Mobilinkd LLC <rob@mobilinkd.com>
// All rights reserved.

#pragma once

#include <ap_axi_sdata.h>
#include <hls_stream.h>
#include <ap_int.h>

typedef ap_axiu<8,0,0,0> odata_type;
typedef ap_axiu<3,0,0,0> idata_type;
typedef hls::stream<odata_type> ostream_type;
typedef hls::stream<idata_type> istream_type;
typedef ap_uint<1> input_type;
typedef ap_uint<1> sample_type;
typedef ap_uint<1> lock_type;
typedef ap_uint<1> cancel_type;

void hdlc_top(ostream_type& output, istream_type& input, cancel_type cancel);
