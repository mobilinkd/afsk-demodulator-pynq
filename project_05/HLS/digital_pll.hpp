#include "hysteresis.hpp"
#include "iir_filter.hpp"

#include "ap_int.h"
#include "hls_math.h"

#include <tuple>

typedef ap_fixed<18,9> fixed_type;

#define ABS(x) ((x) >= (0) ? (x) : (-x))

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

} // pll

template <typename Float = fixed_type>
struct DigitalPLL {

    static const size_t N = 16;

    typedef Float float_type;

    ap_int<16> sample_rate_;
    ap_int<16> symbol_rate_;
    float_type sps_;                        ///< Samples per symbol
    float_type limit_;                      ///< Samples per symbol / 2
    Hysteresis<float_type, ap_int<1>> lock_;

    IirFilter<float_type, 2> loop_filter_;
    IirFilter<float_type, 2> lock_filter_;

    bool last_;
    float_type count_;

    bool sample_;
    float_type jitter_;
    uint8_t bits_;

    typedef std::tuple<bool, bool, bool> result_type;

    DigitalPLL(ap_int<16> sample_rate, ap_int<16> symbol_rate)
    : sample_rate_(sample_rate), symbol_rate_(symbol_rate)
    , sps_(22)
    , limit_(sps_ / 2)
    , lock_(sps_ * float_type(0.025), sps_ * float_type(.15), 1, 0)
    , loop_filter_(pll::loop_b, pll::loop_a)
    , lock_filter_(pll::lock_b, pll::lock_a)
    , last_(false), count_(0), sample_(false)
    , jitter_(0.0), bits_(1)
    {}

    result_type operator()(bool input)
    {

		sample_ = false;

		if (input != last_ or bits_ > 127)
		{
			// Record transition.
			last_ = input;

			if (count_ > limit_)
			{
				count_ -= sps_;
			}

			float_type offset = count_ / bits_;
			float_type jitter = loop_filter_(offset);
			jitter_ = lock_filter_(hls::abs(offset));

			float_type delta = locked() ? float_type(0.012) : float_type(0.048);
			count_ -= jitter * sps_ * delta;
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
		return result_type(input, sample_, locked());
    }

    bool locked() {
        return lock_(jitter_);
    }

    float_type jitter() {
    	return jitter_;
    }

    bool sample() const {
        return sample_;
    }
};
