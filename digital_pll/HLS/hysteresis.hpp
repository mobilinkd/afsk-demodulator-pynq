// Copyright 2019 Mobilinkd LLC <rob@mobilinkd.com>
// All rights reserved.

#pragma once

template <typename Float, typename Int>
struct Hysteresis {

	typedef Float float_type;
	float_type min_;
	float_type max_;

	const Int low_;
	const Int high_;

	float_type last_;

	Hysteresis(float_type minimum, float_type maximum, Int low = 0, Int high = 1)
	: min_(minimum), max_(maximum), low_(low), high_(high), last_(0.0)
	{}

	int operator()(float_type value) {
		if (value <= min_) {
				last_ = low_;
		} else if (value >= max_) {
				last_ = high_;
		}

		return last_;
	}
};

template <typename Float, typename Int>
Hysteresis<Float, Int> make_hystersis(Float min, Float max, Int low, Int high)
{
	return Hysteresis<Float, Int>(min, max, low, high);
}
