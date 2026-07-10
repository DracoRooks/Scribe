#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <cstdint>

namespace Scribe {
    struct PairHash {
        inline size_t operator()(const std::pair<int, int>& v) const {
            return v.first * 997 + v.second;
        }
    };

    using Count = int;
    using Token = int;
    using Pair = std::pair<Token, Token>;
    using PairCounts = std::unordered_map<Pair, Count, PairHash>;
    using WordCounts = std::vector<std::pair<std::vector<Token>, Count>>;

    class BytePairEncoder {
    private:
        std::unordered_map<Token, std::vector<uint8_t>> vocab;
        std::unordered_map<uint8_t, Token> utf8Lookup;
        std::vector<std::pair<Pair, Token>> mergeForest;

        std::vector<Token> getCodePoints(const std::string& str);
        Pair getMostFrequentPair(const WordCounts& wordCounts);
        void doMerge(WordCounts& wordCounts, const Pair& mostFrequentPair, const int newToken);

    public:
        BytePairEncoder();
        ~BytePairEncoder() = default;

        void train(const std::string& filename, int cycles, bool verbose);
        std::vector<Token> encode(const std::string& data);
        std::vector<uint8_t> decode(const std::vector<Token>& tokens);
    };
}
