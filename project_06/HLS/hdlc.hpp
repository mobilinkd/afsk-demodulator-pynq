// Copyright 2015 Mobilinkd LLC <rob@mobilinkd.com>
// All rights reserved.

#include <ap_int.h>
#include <cstdint>
#include "crc.hpp"

#ifndef MOBILINKD___HDLC_DECODER_HPP_
#define MOBILINKD___HDLC_DECODER_HPP_

const ap_uint<16> VALID_CRC = 0xf0b8;
const ap_uint<16> BUFFER_MASK = 0xFF00;

template <uint16_t N>
struct hdlc_decoder
{
private:
    static const uint16_t FLAG = 0x7E00;
    static const uint16_t ABORT = 0x7F;
    static const uint16_t IDLE = 0xFF;

    enum state {SEARCH, HUNT, FRAMING};

    state state_{SEARCH};
    ap_uint<3> ones_{0};
    ap_uint<16> buffer_{0};
    ap_uint<8> (&frame_)[N];
    uint16_t frame_index_{0};
    ap_uint<5> bits_{0};
    bool passall_{false};
    bool ready_{false};
    crc16_ccitt crc_;

public:
    hdlc_decoder(ap_uint<8> (&frame)[N], bool pass_all = false)
	: frame_(frame), passall_(pass_all)
	{}

    /**
     * Process a bit.  If the bit results in a frame, a result set containing
     * the frame, the FCS, and a flag indicating whether it is valid is
     * returned.
     *
     * When PLL is passed, when true it indicates that the bit should be
     * processed, and when false indicates that any frame in progress should
     * be dropped and the state reset to SEARCH.
     *
     * HOWEVER... over the air, it is common for PLL to unlock during the
     * last framing byte for some as yet unknown reason.  Because of this,
     * we attempt to use the data that exists. 868/933.
     */
    void decode_bit(ap_uint<1> bit, bool pll, uint16_t& len, bool& packet, bool& valid)
    {
    	packet = false;
        if (not pll) {
        	if (state_ == FRAMING) {
				len = frame_index_;
				valid = crc_.value() == VALID_CRC;
				packet = passall_ or valid;
        	}
        	ready_ = false;
            start_search();
        } else {
			if (process(bit)) {
				len = frame_index_;
				valid = crc_.value() == VALID_CRC;
				packet = passall_ or valid;
				ready_ = false;
				start_hunt();
			}
        }
    }

    void start_search()
    {
        state_ = SEARCH;
    }

private:
    bool process(bool bit)
    {
        switch (state_)
        {
        case SEARCH:
            search(bit);
            break;
        case HUNT:
            hunt(bit);
            break;
        case FRAMING:
            frame(bit);
            break;
        default:
            start_search();
            break;
        }

        return ready_;
    }

    void add_bit(ap_uint<1> bit)
    {
        buffer_ >>= 1;
        buffer_[15] = bit;
        bits_ += 1;
    }

    ap_uint<8> get_char()
    {
    	ap_uint<8> c = buffer_(7,0);
		for (int j = 0; j != 8; ++j) {
			crc_.compute(c[0]);
			c >>= 1;
		}

        return buffer_(7,0);
    }

    void consume_byte()
    {
        buffer_(7,0) = 0;
        bits_ -= 8;
    }

    void consume_bit()
    {
        ap_uint<8> tmp = buffer_(7,0);
        tmp <<= 1;
        buffer_(7, 0) = tmp;
        bits_ -= 1;
    }

    bool have_flag()
    {
        return buffer_(15,8) == ap_uint<8>(0x7E);
    }

    void start_hunt()
    {
        state_ = HUNT;
        bits_ = 0;
        buffer_ = 0;
    }

    void search(bool bit)
    {
        add_bit(bit);

        if (have_flag())
        {
            start_hunt();
        }
    }

    static ap_uint<2> popcount(ap_uint<4> p)
	{
    	switch (p) {
    	case 15:
    		return 4;
    	case 7:
    	case 11:
    	case 13:
    	case 14:
    		return 3;
    	case 3:
    	case 5:
    	case 6:
    	case 9:
    	case 10:
    	case 12:
    		return 2;
    	case 1:
    	case 2:
    	case 4:
    	case 8:
    		return 1;
    		break;
    	case 0:
    		return 0;
    		break;
    	}
    	return 0;
	}

    bool have_frame_error()
    {
    	ap_uint<3> cnt = popcount(buffer_(15,12)) + popcount(buffer_(11,8));
    	return cnt >= 6;
//        switch (buffer_(15,8))
//        {
//        case 0xFF:
//        case 0xFE:
//        case 0xFC:
//        case 0x7F:
//        case 0x7E:
//        case 0x3F:
//            return true;
//        default:
//            return false;
//        }
    }

    bool have_bogon()
    {
        if  (bits_ != 8) return false;

        switch (buffer_(15,8))
        {
        case 0xFF:
        case 0xFE:
        case 0x7F:
            return true;
        default:
            return false;
        }
    }

    void start_frame()
    {
        state_ = FRAMING;
        frame_index_ = 0;
        crc_.reset();
        ones_ = 0;
        buffer_(7,0) = 0;
    }

    void hunt(bool bit)
    {
        add_bit(bit);
        buffer_(7,0) = 0;

        if (bits_ != 8) return;

        if (have_flag())  {
            start_hunt();
            return;
        }

        if (have_bogon()) {
            start_search();
            return;
        }

        if (not have_frame_error())  {
            start_frame();
            return;
        }

        start_search();
    }

    void frame(ap_uint<1> bit)
    {
        add_bit(bit);

        if (ones_ < 5) {
            if (buffer_[7]) ones_ += 1;
            else ones_ = 0;

            if (bits_[4]) {
                if (frame_index_ < N) {
                	frame_[frame_index_++] = get_char();
                    consume_byte();
                } else {
                    // Allocation error.
                    start_search();
                }
            }

            if (have_flag()) {
                if (frame_index_ > 2) {
                    ready_ = true;
                }
            }
        } else {
            // 5 ones in a row means the next one should be 0 and be skipped.

            if (!buffer_[7]) {
                ones_ = 0;
                consume_bit();
                return;
            } else {
                // Framing error.  Drop the frame.  If there is a FLAG
                // in the buffer, go into HUNT otherwise SEARCH.
                if (frame_index_ > 14) {
                    ready_ = true;
                    return;
                }

                if ((buffer_ >> (16 - bits_) & 0xFF) == 0x7E) {
                    // Cannot call start_hunt() here because we need
                    // to preserve buffer state.
                    bits_ -= 8;
                    state_ = HUNT;
                } else {
                    start_search();
                }
            }
        }
    }
};

#endif // MOBILINKD___HDLC_DECODER_HPP_
