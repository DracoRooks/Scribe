#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include "../include/word_extracter.hpp"

int main() {
    Scribe::WordExtracter wordifier;

    wordifier.wordify("./test/testdata.txt");
    std::unordered_map<std::string, int> words = wordifier.getWords();

    for (auto& word : words) {
        std::cout << '"' << word.first << '"' << " : " << word.second << std::endl;
    }

    std::cout << "Total unique words: " << words.size() << std::endl;

    return 0;
}
