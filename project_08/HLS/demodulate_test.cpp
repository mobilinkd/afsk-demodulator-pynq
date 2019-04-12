#include "demodulate.hpp"
#include <stdlib.h>
#include <math.h>
#include <tuple>
#include <iostream>
#include <sstream>

int crc_test();

#include "audio_data.hpp"

const uint8_t packet[] = {
    0x82, 0xa0, 0xa6, 0x64, 0x64, 0x68, 0x60, 0x96, 0x88, 0x6c, 0x8c, 0xac, 0xa0, 0x64, 0x9c, 0x6c,
    0x8a, 0xb0, 0x40, 0x40, 0xe2, 0xae, 0x92, 0x88, 0x8a, 0x62, 0x40, 0x61, 0x03, 0xf0, 0x3e, 0x31,
    0x35, 0x32, 0x33, 0x34, 0x33, 0x7a, 0x5b, 0x32, 0x32, 0x34, 0x5d, 0x2a, 0x57, 0x65, 0x20, 0x6b,
    0x6e, 0x6f, 0x77, 0x20, 0x6d, 0x6f, 0x73, 0x74, 0x20, 0x6f, 0x66, 0x20, 0x79, 0x6f, 0x75, 0x72,
    0x20, 0x66, 0x61, 0x75, 0x6c, 0x74, 0x73, 0x21, 0x21, 0x21, 0x0d, 0xd5, 0xb3,
};

int main()
{
	istream_type in;
	ostream_type out;

    idata_type tmp_in;
	odata_type tmp_out;

    bool failed = false;
	int count = 0;
	tmp_out.last = 0;
	lock_type locked;
	done_type done = 0;

    for (int j = 0; j != 10; ++j) {
		for (int i = 0; i != 2640; ++i)
		{
			tmp_in.data = audio[i + (j* 2640)];
			tmp_in.last = 0;
			in << tmp_in;
		}

		std::cout << "input size=" <<  in.size() << std::endl;

		demodulate6(in, out, locked, done);

	    while (!out.empty()) {
	    	out >> tmp_out;
	    	uint8_t tmp = tmp_out.data;
	    	std::cout << char(tmp);
	    	if (tmp != packet[count])
	    		std::cout << "(" << tmp << ") != (" << packet[count] << ")" << std::endl;
	    	count += 1;
	    }
	    std::cout << std::endl;
    }


	for (int i = 0; i != 120; ++i)
	{
		tmp_in.data = 0;
		tmp_in.last = 0;
		in << tmp_in;
	}
	tmp_in.data = 0;
	tmp_in.last = 1;
	in << tmp_in;
	demodulate6(in, out, locked, done);

	std::cout << "count = " << count << ", left = " << in.size() << std::endl;

    failed |= count != 77;

    if (!tmp_out.last) {
    	std::cout << "TLAST not set" << std::endl;
    	failed = true;
    }

    failed |= crc_test();

    done = 1;
    demodulate6(in, out, locked, done);

    if (out.empty()) {
    	failed = 1;
    } else {
    	out >> tmp_out;
    	failed |= (tmp_out.data != 0);
    	failed |= (tmp_out.last != 1);
    }

    return failed;
}

