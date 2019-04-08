#include "hysteresis.hpp"
#include "iir_filter.hpp"

#include "ap_int.h"
#include "hls_math.h"

#include <tuple>

typedef ap_fixed<32,20> fixed_type;

namespace pll {

// Loop low-pass filter taps (64Hz Bessel)
const fixed_type loop_b[] = {
    0.144668495309,
    0.144668495309,
};
const fixed_type loop_a[] = {
    1.0,
    -0.710663009381,
};

// Lock low-pass filter taps (40Hz Bessel)
const fixed_type lock_b[] = {
    0.0951079834025,
    0.0951079834025,
};
const fixed_type lock_a[] = {
    1.0,
    -0.809784033195,
};

/*
 * FIR Filter coeffs.
const fixed_type loop_coeff[] = {0.0,0.24765178738120416,0.5046964252375916,0.24765178738120416,0.0};
const fixed_type lock_coeff[] = {0.0,0.24908481189362192,0.5018303762127563,0.24908481189362192,0.0};
 */
} // pll


template <typename Float = fixed_type>
struct DigitalPLL {

    typedef Float float_type;

    float_type sample_rate_;
    float_type symbol_rate_;
    float_type sps_;                        ///< Samples per symbol
    float_type limit_;                      ///< Samples per symbol / 2
    Hysteresis<float_type, ap_int<1>> lock_;

    IirFilter<float_type, 2> loop_filter_{pll::loop_b, pll::loop_a};
    IirFilter<float_type, 2> lock_filter_{pll::lock_b, pll::lock_a};

    bool locked_{false};

    bool last_;
    float_type count_;

    bool sample_;
    float_type jitter_;
    ap_uint<8> bits_;

    typedef std::tuple<bool, bool, bool> result_type;

    DigitalPLL(float_type sample_rate, float_type symbol_rate)
    : sample_rate_(sample_rate), symbol_rate_(symbol_rate)
    , sps_(sample_rate / symbol_rate), limit_(sps_ >> 1)
    , lock_(sps_ * float_type(0.025), sps_ * float_type(0.15), 1, 0)
    , last_(false), count_(0), sample_(false)
    , jitter_(0.0), bits_(1)
    {}

    void pll(bool input, bool& sample_out, bool& locked_out)
    {
		sample_ = false;

		if (input != last_ or bits_[7] == 1)
		{
			// Record transition.
			last_ = input;

			ap_uint<1> limited = static_cast<ap_uint<1>>(count_ > limit_);
			count_ -= sps_ * limited;

			float_type offset = count_ / bits_;
			float_type jitter = loop_filter_(offset);
			jitter_ = lock_filter_(hls::abs(offset));

			locked_ = lock_(jitter_);
			float_type delta = locked_ ? float_type(0.012) : float_type(0.048);
			auto tmp = sps_ * delta;
			count_ -= jitter * tmp;
			bits_ = 1;
		}
		else
		{
			if (count_ > limit_)
			{
				sample_ = true;
				count_ -= sps_;
				++bits_;
			}
		}

		count_ += 1;
		locked_out = locked_;
		sample_out = sample_;
    }
};
