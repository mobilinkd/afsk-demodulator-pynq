#include "demodulate.hpp"

#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <numeric>
#include <algorithm>

int main()
{
        stream_type input[BLOCK_SIZE];
        stream_type output[BLOCK_SIZE];

        for (int i = 0; i != BLOCK_SIZE; ++i)
        {
                input[i].data = 16;
        }

        demodulate(input, output);

        int accum = 0;
        for (int i = 0; i != BLOCK_SIZE; ++i)
        {
                accum += output[i].data;
        }

        printf("total=%d, expected=%d", accum, BLOCK_SIZE*10);

        if (accum == BLOCK_SIZE * 10) return EXIT_SUCCESS;

        return EXIT_FAILURE;
}
