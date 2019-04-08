#include <ap_axi_sdata.h>
#include <hls_stream.h>
#include <stdint.h>

#define BPF_COEFF_LEN 141

typedef ap_axis<16,1,1,1> idata_type;
typedef hls::stream<idata_type> istream_type;
typedef ap_axiu<8,1,1,1> odata_type;
typedef hls::stream<odata_type> ostream_type;
typedef ap_int<1> lock_type;
typedef int done_type;

void demodulate6(istream_type& input, ostream_type& output, lock_type& lock, done_type done);
