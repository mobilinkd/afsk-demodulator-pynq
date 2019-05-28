// Copyright 2019 Mobilinkd LLC <rob@mobilinkd.com>
// All rights reserved.

#pragma once

#include <ap_axi_sdata.h>
#include <hls_stream.h>
#include <stdint.h>

using idata_type = ap_axiu<1,0,0,0>;
using odata_type = ap_axiu<3,0,0,0>;
using istream_type = hls::stream<idata_type>;
using ostream_type = hls::stream<odata_type>;
using locked_type = ap_int<1>;

void demodulate_top(istream_type& input, ostream_type& output, locked_type& locked);
