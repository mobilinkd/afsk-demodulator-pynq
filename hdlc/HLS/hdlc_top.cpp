#include "hdlc_top.hpp"
#include "hdlc.hpp"
#include "nrzi.hpp"

void hdlc_top(ostream_type& output, istream_type& input, cancel_type cancel)
{
#pragma HLS INTERFACE axis port=output
#pragma HLS INTERFACE axis port=input
#pragma HLS INTERFACE s_axilite port=cancel bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

	static nrzi nrzi_;

	static odata_type odata;
	static idata_type idata;

	odata.strb = 1;

	ap_uint<1> bit;
	ap_uint<1> sample;
	ap_uint<1> locked;

	if (cancel) {
		hdlc(odata.data, odata.keep, odata.last, 0, 0, cancel);
		output << odata;
	} else {
		while (!input.empty())
		{
#pragma HLS PIPELINE
			input >> idata;

			bit = idata.data[2];
			sample = idata.data[1];
			locked = idata.data[0];

			if (sample) {
				hdlc(odata.data, odata.keep, odata.last, nrzi_.decode(bit), locked, cancel);
				if (odata.keep) {
					output << odata;
				}
			}
		}
	}
}
