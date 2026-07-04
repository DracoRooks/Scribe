#pragma once

#include <string>
#include <unordered_map>

namespace Scribe {
    class WordExtracter
    {
    private:
        int getUTFByteLength(const char ch);

    public:
        WordExtracter() = default;
        ~WordExtracter() = default;

        std::unordered_map<std::string, int> wordify(const std::string& filename);
    };
}
