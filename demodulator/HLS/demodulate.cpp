// Copyright 2019 Mobilinkd LLC <rob@mobilinkd.com>
// All rights reserved.

#include "demodulate.hpp"
#include "digital_pll.hpp"
#include "hdlc.hpp"
#include "nrzi.hpp"
#include "fir_filter.hpp"

#include <iostream>
#include <array>

#define BPF_COEFF_LEN 129
#define LPF_COEFF_LEN 95

namespace mobilinkd {

const ap_int<13> bpf_coeffs[] =
{
	     1,     3,     5,     8,     8,     5,    -2,   -13,   -27,   -40,   -46,   -44,
	   -32,   -12,    11,    32,    44,    44,    32,    14,     0,    -2,    13,    49,
	    97,   143,   170,   160,   104,     6,  -118,  -244,  -340,  -381,  -352,  -258,
	  -120,    24,   138,   192,   173,    97,     0,   -67,   -56,    62,   287,   575,
	   850,  1021,  1001,   737,   228,  -462, -1216, -1879, -2293, -2336, -1956, -1182,
	  -133,  1008,  2030,  2736,  2988,  2736,  2030,  1008,  -133, -1182, -1956, -2336,
	 -2293, -1879, -1216,  -462,   228,   737,  1001,  1021,   850,   575,   287,    62,
	   -56,   -67,     0,    97,   173,   192,   138,    24,  -120,  -258,  -352,  -381,
	  -340,  -244,  -118,     6,   104,   160,   170,   143,    97,    49,    13,    -2,
	     0,    14,    32,    44,    44,    32,    11,   -12,   -32,   -44,   -46,   -40,
	   -27,   -13,    -2,     5,     8,     8,     5,     3,     1,
};

const ap_int<13> lpf_coeffs[] =
{
	     1,     3,     5,     8,    11,    14,    17,    20,    21,    20,    17,    11,
	     2,    -9,   -25,   -44,   -66,   -91,  -116,  -142,  -167,  -188,  -205,  -215,
	  -217,  -209,  -190,  -156,  -109,   -47,    30,   123,   230,   350,   481,   622,
	   769,   919,  1070,  1217,  1358,  1488,  1605,  1704,  1785,  1844,  1880,  1893,
	  1880,  1844,  1785,  1704,  1605,  1488,  1358,  1217,  1070,   919,   769,   622,
	   481,   350,   230,   123,    30,   -47,  -109,  -156,  -190,  -209,  -217,  -215,
	  -205,  -188,  -167,  -142,  -116,   -91,   -66,   -44,   -25,    -9,     2,    11,
	    17,    20,    21,    20,    17,    14,    11,     8,     5,     3,     1,
};

DigitalPLL<> dpll(26400.0, 1200.0);

void demod(ap_int<16> data, ap_uint<1>& bit, ap_uint<1>& sample_out, ap_uint<1>& locked_out)
{
#pragma HLS INLINE
	static ap_uint<12> delay_line{0};
#pragma HLS RESOURCE variable=delay_line core=FIFO_SRL

    static ap_int<16> bpf_shift_reg[BPF_COEFF_LEN];
#pragma HLS ARRAY_PARTITION variable=bpf_shift_reg complete dim=1
    static ap_int<2> lpf_shift_reg[LPF_COEFF_LEN];
#pragma HLS ARRAY_PARTITION variable=lpf_shift_reg complete dim=1

	auto bpfiltered = fir_filter(data, bpf_shift_reg, bpf_coeffs);
	ap_uint<1> comp = bpfiltered >= 0;
	ap_uint<1> delayed = delay_line[11];
	delay_line <<= 1;
	delay_line[0] = comp;
	ap_int<2> corr = comp ^ delayed;
	ap_int<2> corr_norm = (corr << 1) - 1;
	auto lpfiltered = fir_filter(corr_norm, lpf_shift_reg, lpf_coeffs, 2);
	bit = lpfiltered >= 0 ? 1 : 0;
	dpll.pll(bit, sample_out, locked_out);
}

void demodulate(
	odata_type& output, valid_type& valid, last_type& last, lock_type& lock,
	idata_type input, cancel_type cancel)
{
	static nrzi afsk_nrzi;

	is_done: if (cancel) {
		lock = 0;
		hdlc(output, valid, last, 0, 0, cancel);
	} else {
		ap_uint<1> bit;
		ap_uint<1> sample;
		ap_uint<1> locked;

		demod(input, bit, sample, locked);

		lock = locked;

		should_sample: if (sample == 1) {
			ap_uint<1> nzbit = afsk_nrzi.decode(bit);
			hdlc(output, valid, last, nzbit, locked, cancel);
		} else {
			output = 0;
			valid = 0;
			last = 0;
		}
	}
}

} // mobilinkd

