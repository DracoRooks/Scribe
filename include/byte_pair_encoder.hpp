#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <cstdint>

#include <./typedefs.hpp>

namespace Scribe {
    class BytePairEncoder {
    private:
        std::unordered_map<Token, std::vector<uint8_t>> vocab;
        std::unordered_map<uint8_t, Token> utf8Lookup;
        std::vector<std::pair<Pair, Token>> mergeForest;

        std::vector<Token> getCodePoints(const std::string& str);
        Pair getMostFrequentPair(const WordCounts& wordCounts);
        Pair getBestRankedPair(const WordCounts& wordCounts);
        void doMerge(WordCounts& wordCounts, const Pair& pairToMerge, const int newToken);

    public:
        BytePairEncoder();
        ~BytePairEncoder() = default;

        void train(const std::string& filename, int cycles, bool normalizedRanking, bool verbose);
        std::vector<Token> encode(const std::string& data);
        std::vector<uint8_t> decode(const std::vector<Token>& tokens);
    };
}
