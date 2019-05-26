// Copyright 2019 Mobilinkd LLC.

#pragma once

#include <ap_axi_sdata.h>
#include <hls_stream.h>
#include <ap_int.h>

#define BPF_COEFF_LEN 129

typedef ap_axis<16,0,0,0> iodata_type;
typedef hls::stream<iodata_type> stream_type;

void demodulate(stream_type& input, stream_type& output);
