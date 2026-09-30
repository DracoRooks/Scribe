#include "../include/byte_pair_encoder.hpp"

#include <iostream>
#include <cstdint>

int main() {
    Scribe::BytePairEncoder bpe;
    bpe.train("./test/testdata.txt", 34, false, true);

    std::cout << "Sample Data: Hello! testing... 🫡" << std::endl;
    const std::vector<int> encodedData = bpe.encode("Hello! testing... 🫡");
    std::cout << "Encoded Data: [";
    for (const auto token : encodedData) std::cout << token << ", ";
    std::cout << "]" << std::endl;
    const std::string str(bpe.decode(encodedData));
    std::cout << "Decoded Data: '" << str << "'" << std::endl;
}
