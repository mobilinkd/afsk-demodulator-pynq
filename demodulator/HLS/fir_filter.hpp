// Copyright 2019 Mobilinkd LLC <rob@mobilinkd.com>
// All rights reserved.

#ifndef MOBILINKD__FIR_FILTER_HPP_
#define MOBILINKD__FIR_FILTER_HPP_

#pragma once

#include "ap_int.h"
#include "hls_math.h"

#include <cstddef>
#include <array>

template <typename InOut, typename Filter, size_t N>
inline ap_int<16> fir_filter(InOut x, InOut (&shift_reg)[N], Filter (&coeff)[N], size_t shift = (InOut::width - 1))
{
#pragma HLS pipeline II=2
    ap_int<36> accum{0};
    filter_loop: for (size_t i = N-1 ; i != 0; i--)
    {
    	InOut tmp = shift_reg[i-1];
        shift_reg[i] = tmp;
        ap_int<InOut::width + Filter::width> tmp2 = tmp * coeff[i];
        accum += tmp2;
    }

    shift_reg[0] = x;
    accum += (x * coeff[0]);

    return static_cast<ap_int<16>>(accum >> shift);
}

template <typename InOut, typename Filter, size_t N>
inline InOut fixed_fir_filter(InOut data, InOut (&shift_reg)[N], const std::array<Filter, N>& coeffs)
{
	ap_fixed<36, InOut::iwidth> accum = 0;
    fixed_filter_loop: for (size_t i = N-1 ; i != 0; i--)
    {
#pragma HLS pipeline II=2
    	InOut tmp = shift_reg[i-1];
        shift_reg[i] = shift_reg[i-1];
        accum += tmp * coeffs[i];
    }

    shift_reg[0] = data;
    accum += data * coeffs[0];

    return static_cast<InOut>(accum);
}

#endif // MOBILINKD__FIR_FILTER_HPP_
