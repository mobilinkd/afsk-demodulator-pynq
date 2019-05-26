#include <ap_int.h>

typedef ap_uint<4> leds_type;
typedef ap_uint<4> btns_type;

ap_uint<1> count(volatile leds_type& leds_out, int& cntr, btns_type btn_in);

class Debouncer {
	ap_uint<17> counter;
public:
	void start() {counter = 0;}
	void inc() {if (counter[16] != 1) ++counter;}
	bool ok() const {return counter[16] == 1;}
};

class Button {
	enum State { UP, DEBOUNCE, DOWN };
	Debouncer debounce_;
	State state_;
public:
	Button() : state_(UP) {}

	template <typename T>
	T state(T button) {

		T result = 0;

		switch (state_) {
		case UP:
			if (button) {
				debounce_.start();
				state_ = DEBOUNCE;
			}
			break;
		case DEBOUNCE:
			debounce_.inc();
			if (debounce_.ok()) {
				if (button) {
					state_ = DOWN;
					result = button;
				} else {
					state_ = UP;
				}
			}
			break;
		case DOWN:
			if (!button) {
				debounce_.start();
				state_ = DEBOUNCE;
			}
		}
		return result;
	}
};

