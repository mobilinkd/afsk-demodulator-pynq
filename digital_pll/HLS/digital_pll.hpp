// Copyright 2019 Mobilinkd LLC <rob@mobilinkd.com>
// All rights reserved.

#pragma once

#include "hysteresis.hpp"
#include "fir_filter.hpp"

#include "ap_int.h"
#include "ap_fixed.h"
#include "hls_math.h"

#include <tuple>
#include <array>
#include <iostream>

using fixed_type = ap_fixed<18, 6>;
using coeff_type = ap_fixed<18, 2>;
using inverse_type = ap_fixed<18, 2>;

namespace pll {

const std::array<inverse_type, 16> inverse = {
	1.0, 1.0/2.0, 1.0/3.0, 1.0/4.0, 1.0/5.0, 1.0/6.0, 1.0/7.0, 1.0/8.0,
	1.0/9.0, 1.0/10.0, 1.0/11.0, 1.0/12.0, 1.0/13.0, 1.0/14.0, 1.0/15.0, 1.0/16.0,
};

/*
 * FIR Filter coeffs.
 */
// 64 Hz loop filter.
const std::array<coeff_type, 5> loop_coeffs = {
		0.08160962754214955, 0.25029850550446403, 0.3361837339067726, 0.2502985055044641, 0.08160962754214969
};
// 40 Hz lock filter.
const std::array<coeff_type, 5> lock_coeffs = {
		0.07893082388823802, 0.25073951353226703, 0.34065932515898967, 0.25073951353226714, 0.07893082388823815
};

} // pll


const fixed_type ZERO = 0.0;

template <typename Float = fixed_type>
struct DigitalPLL {

    typedef Float float_type;
    using count_type = ap_fixed<27, 9>;

    float_type sps_;                        ///< Samples per symbol
    count_type limit_;                      ///< Samples per symbol / 2
    Hysteresis<float_type, ap_int<1>> lock_;

    float_type loop_shift_reg[5];
    float_type lock_shift_reg[5];

    ap_uint<1> locked_{0};

    ap_uint<1> last_;
    count_type count_;

    ap_uint<1> sample_;
    float_type jitter_;
    ap_uint<8> bits_;

    typedef std::tuple<bool, bool, bool> result_type;

    DigitalPLL(ap_fixed<18,17> sample_rate, ap_fixed<18,17> symbol_rate)
    : sps_(sample_rate / symbol_rate), limit_(sps_ >> 1)
    , lock_(sps_ * float_type(0.025), sps_ * float_type(0.15), 1, 0)
    , last_(0), count_(0), sample_(0)
    , jitter_(0.0), bits_(1)
    {
#pragma HLS ARRAY_PARTITION variable=loop_shift_reg complete dim=1
#pragma HLS ARRAY_PARTITION variable=lock_shift_reg complete dim=1
#ifndef __SYNTHESIS__
    	for (auto x : pll::inverse) {
    		std::cout << x << " ";
    	}
    	std::cout << std::endl;
    	std::cout << "   SPS: " << sps_ << std::endl;
    	std::cout << " Limit: " << limit_ << std::endl;
    	std::cout << "  Lock: " << sps_ * float_type(0.025) << std::endl;
    	std::cout << "Unlock: " << sps_ * float_type(0.15) << std::endl;
#endif

    }

    void pll(ap_uint<1> input, ap_uint<1>& sample_out, ap_uint<1>& locked_out)
    {

		sample_ = 0;

		float_type count_change = -1.0;

		if (input != last_ or bits_[4] == 1)
		{
			// Record transition.
			last_ = input;

			float_type limited = (count_ > limit_ ? sps_ : ZERO);

			auto inverse = pll::inverse[(bits_ - 1) & 0x0F];
			float_type offset = count_ * inverse;
			float_type abs_offset = offset >= 0 ? (offset - ZERO) : (ZERO - offset);
			float_type jitter = fixed_fir_filter(offset, loop_shift_reg, pll::loop_coeffs);
			jitter_ = fixed_fir_filter(abs_offset, lock_shift_reg, pll::lock_coeffs);

			locked_ = lock_(jitter_);
			float_type delta = locked_ ? float_type(0.012) : float_type(0.048);
			auto tmp = sps_ * delta;
			count_change = jitter * tmp + limited - 1;
			bits_ = 1;
		}
		else
		{
			if (count_ >= limit_)
			{
				sample_ = 1;
				count_change = sps_ - 1;
				++bits_;
			}
		}

		count_ = count_ - count_change;
		locked_out = locked_;
		sample_out = sample_;
    }
};
