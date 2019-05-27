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

const std::array<inverse_type, 128> inverse = {
	1.0, 1.0/2.0, 1.0/3.0, 1.0/4.0, 1.0/5.0, 1.0/6.0, 1.0/7.0, 1.0/8.0,
	1.0/9.0, 1.0/10.0, 1.0/11.0, 1.0/12.0, 1.0/13.0, 1.0/14.0, 1.0/15.0, 1.0/16.0,
#if 0
	1.0/17.0, 1.0/18.0, 1.0/19.0, 1.0/20.0, 1.0/21.0, 1.0/22.0, 1.0/23.0, 1.0/24.0,
	1.0/25.0, 1.0/26.0, 1.0/27.0, 1.0/28.0, 1.0/29.0, 1.0/30.0, 1.0/31.0, 1.0/32.0,
	1.0/33.0, 1.0/34.0, 1.0/35.0, 1.0/36.0, 1.0/37.0, 1.0/38.0, 1.0/39.0, 1.0/40.0,
	1.0/41.0, 1.0/42.0, 1.0/43.0, 1.0/44.0, 1.0/45.0, 1.0/46.0, 1.0/47.0, 1.0/48.0,
	1.0/49.0, 1.0/50.0, 1.0/51.0, 1.0/52.0, 1.0/53.0, 1.0/54.0, 1.0/55.0, 1.0/56.0,
	1.0/57.0, 1.0/58.0, 1.0/59.0, 1.0/60.0, 1.0/61.0, 1.0/62.0, 1.0/63.0, 1.0/64.0,
	1.0/65.0, 1.0/66.0, 1.0/67.0, 1.0/68.0, 1.0/69.0, 1.0/70.0, 1.0/71.0, 1.0/72.0,
	1.0/73.0, 1.0/74.0, 1.0/75.0, 1.0/76.0, 1.0/77.0, 1.0/78.0, 1.0/79.0, 1.0/80.0,
	1.0/81.0, 1.0/82.0, 1.0/83.0, 1.0/84.0, 1.0/85.0, 1.0/86.0, 1.0/87.0, 1.0/88.0,
	1.0/89.0, 1.0/90.0, 1.0/91.0, 1.0/92.0, 1.0/93.0, 1.0/94.0, 1.0/95.0, 1.0/96.0,
	1.0/97.0, 1.0/98.0, 1.0/99.0, 1.0/100.0, 1.0/101.0, 1.0/102.0, 1.0/103.0, 1.0/104.0,
	1.0/105.0, 1.0/106.0, 1.0/107.0, 1.0/108.0, 1.0/109.0, 1.0/110.0, 1.0/111.0, 1.0/112.0,
	1.0/113.0, 1.0/114.0, 1.0/115.0, 1.0/116.0, 1.0/117.0, 1.0/118.0, 1.0/119.0, 1.0/120.0,
	1.0/121.0, 1.0/122.0, 1.0/123.0, 1.0/124.0, 1.0/125.0, 1.0/126.0, 1.0/127.0, 1.0/128.0,
#endif
};

/*
 * FIR Filter coeffs.
 */
// 64 Hz loop filter.
const std::array<coeff_type, 5> loop_coeffs = {
		0.0, 0.249995166364, 0.500009667272, 0.249995166364, 0.0
};
// 40 Hz lock filter.
const std::array<coeff_type, 5> lock_coeffs = {
		0.0, 0.24999811187, 0.50000377626, 0.24999811187, 0.0
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
#pragma HLS PIPELINE II=3

		sample_ = 0;

		float_type count_change = 0.0;

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
			count_change = jitter * tmp + limited;
			bits_ = 1;
		}
		else
		{
			if (count_ >= limit_)
			{
				sample_ = 1;
				count_change = sps_;
				++bits_;
			}
		}

		count_ = (count_ + 1) - count_change;
		locked_out = locked_;
		sample_out = sample_;
    }
};
