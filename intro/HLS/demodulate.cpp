#include "demodulate.hpp"

#include <stdint.h>

void demodulate(stream_type input[BLOCK_SIZE], stream_type output[BLOCK_SIZE])
{
#pragma HLS INTERFACE axis port=input
#pragma HLS INTERFACE axis port=output
#pragma HLS INTERFACE ap_none port=return

    for (int i = 0; i != BLOCK_SIZE; ++i) {
            output[i].user = input[i].user;
            output[i].last = input[i].last;
            output[i].strb = input[i].strb;
            output[i].keep = input[i].keep;
            output[i].id = input[i].id;
            output[i].dest = input[i].dest;

            int16_t t = input[i].data;
            int16_t r = ((t * 5) >> 3);
            output[i].data = r;
    }
}
