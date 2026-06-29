#pragma once

#include <string>
#include <unordered_map>

namespace Scribe {
    class WordExtracter
    {
    private:
        std::unordered_map<std::string, int> words;

    public:
        WordExtracter() = default;
        ~WordExtracter() = default;

        void wordify(const std::string& filename);
        std::unordered_map<std::string, int> getWords() { return words; }
    };
}
