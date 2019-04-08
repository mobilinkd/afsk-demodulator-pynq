#ifndef MOBILINKD__NRZI_H_
#define MOBILINKD__NRZI_H_

struct nrzi {

        bool state_;
        
        nrzi()
        : state_(false)
        {}
        
        bool decode(bool x) {
                bool result = (x == state_);
                state_ = x;
                return result;
        }

        bool encode(bool x) {
            if (x == 0) {
                state_ ^= 1;    // Flip the bit.
            }
            return state_;
        }
};

#endif // MOBILINKD__NRZI_H_

