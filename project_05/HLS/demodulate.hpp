#include <ap_axi_sdata.h>
#include <hls_stream.h>
#include <stdint.h>

#define BPF_COEFF_LEN 141

typedef ap_axis<16,1,1,1> idata_type;
typedef ap_axis<3,1,1,1> odata_type;

void demodulate5(idata_type& input, odata_type& output);
