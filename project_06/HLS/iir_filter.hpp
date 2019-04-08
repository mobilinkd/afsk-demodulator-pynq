#include <cstddef>

template <typename Float, size_t N>
struct IirFilter {

	typedef Float float_type;
	const float_type* numerator_;
	const float_type* denominator_;
	float_type history_[N];
	size_t size_;

    IirFilter(const float_type* b, const float_type* a)
    : numerator_(b), denominator_(a)
    , history_(), size_(N)
    {}

    ~IirFilter() {}

    float_type operator()(float_type input)
    {
		for(size_t i = size_ - 1; i != 0; i--) {
			history_[i] = history_[i - 1];
		}

		history_[0] = input;

		for (size_t i = 1; i != size_; i++) {
				history_[0] -= denominator_[i] * history_[i];
		}

		float_type result = 0;
		for (size_t i = 0; i != size_; i++) {
				result += numerator_[i] * history_[i];
		}

		return result;
    }
};

template <typename F, size_t N>
IirFilter<F,N> make_iir_filter(F (&b)[N], F (&a)[N])
{
	return IirFilter<F,N>(b, a);
}
