#include <ap_axi_sdata.h>
#include <hls_stream.h>
#include <stdint.h>

#define BPF_COEFF_LEN 141

typedef hls::stream<int16_t> instream_type;
typedef ap_axis<1,1,1,1> odata_type;
typedef hls::stream<odata_type> outstream_type;

void demodulate3(instream_type& input, outstream_type& output);
