#ifndef MOBILINKD__NRZI_H_
#define MOBILINKD__NRZI_H_

#include "ap_int.h"

struct nrzi {

	ap_uint<1> state_;
        
	nrzi()
	: state_(0)
	{}

	ap_uint<1> decode(ap_uint<1> x) {
		ap_uint<1>  result = (x == state_);
		state_ = x;
		return result;
	}

	ap_uint<1> encode(ap_uint<1> x) {
		if (x == 0) {
			state_ ^= 1;    // Flip the bit.
		}
		return state_;
	}
};

#endif // MOBILINKD__NRZI_H_

