#include <ap_axi_sdata.h>
#include <hls_stream.h>
#include <stdint.h>

#define BPF_COEFF_LEN 141
#define BLOCK_SIZE 264

typedef ap_axis<16,1,1,1> stream_type;

void demodulate2(stream_type input[BLOCK_SIZE], stream_type output[BLOCK_SIZE]);
