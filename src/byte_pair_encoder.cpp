#include "../include/byte_pair_encoder.hpp"

Scribe::BytePairEncoder::BytePairEncoder() {
    for (int i = 0; i < 256; i++) {
        vocab[i] = { static_cast<u_int8_t>(i) };
    }
}
