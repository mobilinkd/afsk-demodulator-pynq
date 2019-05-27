// Copyright 2019 Mobilinkd LLC <rob@mobilinkd.com>
// All rights reserved.

#pragma once

#include <ap_int.h>
#include "crc.hpp"

typedef ap_uint<8> output_type;
typedef ap_uint<1> valid_type;
typedef ap_uint<1> last_type;
typedef ap_uint<1> input_type;
typedef ap_uint<1> lock_type;
typedef ap_uint<1> cancel_type;

const ap_uint<8> STATUS_OK{0x01};
const ap_uint<8> STATUS_USER_CANCEL{0x02};
const ap_uint<8> STATUS_FRAME_ABORT{0x04};
const ap_uint<8> STATUS_FRAME_ERROR{0x08};
const ap_uint<8> STATUS_NO_CARRIER{0x10};
const ap_uint<8> STATUS_CRC_ERROR{0x20};

const ap_uint<16> VALID_CRC = 0xf0b8;

inline void hdlc(
	output_type& output, valid_type& valid_, last_type& last,
	input_type input, lock_type pll_lock, cancel_type cancel)
{
	enum class State {IDLE, SYNC, RECEIVE};

	static State state{State::IDLE};

	static ap_uint<8> buffer{0};

	static ap_uint<3> bits{0};
	static ap_uint<3> ones{0};
	static ap_uint<1> flag{0};
	static crc16_ccitt crc;

	ap_uint<1> valid = 0;

	output = 0;
	last = 0;

#pragma HLS PIPELINE II=2

	if (cancel) {
		state = State::IDLE;
		buffer = 0;
		bits = 0;
		ones = 0;
		flag = 0;
		output = STATUS_USER_CANCEL;
		valid = 1;
		last = 1;
	} else {
		if (pll_lock) {
			if (ones == 5) {
				if (input) {
					// flag byte
					flag = 1;
				} else {
					// bit stuffing...
					flag = 0;
					ones = 0;
					valid_ = valid;
					return;
				}
			}

			buffer >>= 1;
			buffer[7] = input;
			bits += 1;						// Free-running until Sync byte.
			if (input) {
				++ones;
			} else {
				ones = 0;
			}

			HDLC_FSM: switch(state) {

			case State::IDLE:
				if (flag && buffer == 0x7E) {
					state = State::SYNC;
					crc.reset();			// Reset at transition to sync.
					ones = 0;
					flag = 0;
					bits = 0;
				}
				break;

			case State::SYNC:
				if (bits == 0) {	// 8th bit.
					if (flag) {
						if (buffer != 0x7E) {
							state = State::IDLE;
						}
						buffer = 0;
					} else {
						// Start of frame data.
						state = State::RECEIVE;
						output = buffer;
						valid = 1;
						last = 0;
						crc_loop_1: for (int j = 0; j != 8; ++j) {
							crc.compute(buffer[0]);
							buffer >>= 1;
						}
					}
					flag = 0;
					bits = 0;
				}
				break;

			case State::RECEIVE:
				if (bits == 0) {	// 8th bit.
					valid = 1;
					if (flag) {
						last = 1;
						switch (buffer) {
						case 0x7E:
							// End of frame
							output = crc.value() == VALID_CRC ? STATUS_OK : STATUS_CRC_ERROR;
							state = State::SYNC;
							crc.reset();			// Reset at transition to sync.
							break;
						case 0xFE:
						case 0x7F:
							// Abort
							state = State::IDLE;
							output = STATUS_FRAME_ABORT;
							break;
						default:
							// Framing error
							state = State::IDLE;
							output = STATUS_FRAME_ERROR;
							break;
						}
					} else {
						state = State::RECEIVE;
						output = buffer;
						last = 0;
						crc_loop_2: for (int j = 0; j != 8; ++j) {
							crc.compute(buffer[0]);
							buffer >>= 1;
						}
					}
					valid = 1;
					flag = 0;
					bits = 0;
				}
			}
		} else {
			// PLL unlocked.
			if (state == State::RECEIVE) {
				output = crc.value() == VALID_CRC ? STATUS_OK : STATUS_NO_CARRIER;
				valid = 1;
				last = 1;
			}
			state = State::IDLE;
		}

		if (valid) {
			buffer = 0;
		}
	}

	valid_ = valid;
}

