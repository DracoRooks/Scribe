#include "../include/byte_pair_encoder.hpp"

#include <iostream>
#include <cstdint>

int main() {
    Scribe::BytePairEncoder bpe;
    bpe.train("./test/testdata.txt", 34, true);

    std::cout << "Sample Data: Hello! testing... 🫡" << std::endl;
    const std::vector<int> encodedData = bpe.encode("Hello! testing... 🫡");
    std::cout << "Encoded Data: [";
    for (const auto token : encodedData) std::cout << token << ", ";
    std::cout << "]" << std::endl;
    const std::vector<uint8_t> decodedData = bpe.decode(encodedData);
    const std::string str(decodedData.begin(), decodedData.end());
    std::cout << "Decoded Data: '" << str << "'" << std::endl;
}
