#include <ap_axi_sdata.h>
#include <hls_stream.h>

#define BLOCK_SIZE 264

typedef ap_axis<16,1,1,1> stream_type;

void demodulate(stream_type input[BLOCK_SIZE], stream_type output[BLOCK_SIZE]);
