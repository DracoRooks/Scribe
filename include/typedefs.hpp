#pragma once

#include <unordered_map>
#include <vector>

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
}
