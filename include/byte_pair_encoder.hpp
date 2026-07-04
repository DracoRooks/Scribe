#pragma once

#include <unordered_map>
#include <iostream>

namespace Scribe {
    struct pairHash {
        inline size_t operator()(const std::pair<int, int>& v) const {
            return v.first * 997 + v.second;
        }
    };

    class BytePairEncoder {
    private:
        std::unordered_map<int, u_int8_t> vocab;
        std::unordered_map<std::pair<int, int>, int, pairHash> mergeForest;

    public:
        BytePairEncoder();
        ~BytePairEncoder() = default;
    };
}
