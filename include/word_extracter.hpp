#pragma once

#include <vector>
#include <string>

namespace Scribe {
    class WordExtracter
    {
    private:
        std::vector<std::string_view> words;

    public:
        WordExtracter();
        ~WordExtracter() = default;
    };
}
