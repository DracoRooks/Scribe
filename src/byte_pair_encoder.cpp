#include "../include/byte_pair_encoder.hpp"

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>

#include "../include/word_extracter.hpp"

std::vector<Scribe::Token> Scribe::BytePairEncoder::getCodePoints(const std::string& str) {
    int len = str.length();
    int i;

    std::vector<Token> codepoints(len);

    for (i = 0; i < len; i++) {
        codepoints[i] = utf8Lookup[str[i]];
    }

    return codepoints;
}

Scribe::Pair Scribe::BytePairEncoder::getMostFrequentPair(const WordCounts& wordCounts) {
    PairCounts pairCounts;
    Count maxCount = 0;
    Pair mostFrequentPair = { -1, -1 };

    for (const auto& [word, count] : wordCounts) {
        int len = word.size();
        if (len < 2) continue;

        int i = 0, j = 1;
        while (j < len) {
            Pair pair = { word[i], word[j] };
            pairCounts[pair]++;

            if (pairCounts[pair] > maxCount) {
                maxCount = pairCounts[pair];
                mostFrequentPair = pair;
            }

            i++; j++;
        }
    }

    return mostFrequentPair;
}

void Scribe::BytePairEncoder::doMerge(WordCounts& wordCounts, const Pair& mostFrequentPair, const int newToken) {
    for (auto& [word, _] : wordCounts) {
        int len = word.size();
        int read_idx = 0;
        int write_idx = 0;

        while (read_idx < len) {
            if (read_idx + 1 < len && word[read_idx] == mostFrequentPair.first && word[read_idx + 1] == mostFrequentPair.second) {
                word[read_idx] = newToken;
                word[write_idx++] = std::move(word[read_idx]);

                read_idx += 2;
            } else {
                if (write_idx != read_idx) {
                    word[write_idx] = std::move(word[read_idx]);
                }
                write_idx++;
                read_idx++;
            }
        }
        word.resize(write_idx);
    }
}

Scribe::BytePairEncoder::BytePairEncoder() {
    for (int i = 0; i < 256; i++) {
        const u_int8_t ch = static_cast<u_int8_t>(i);

        vocab[i] = { ch };
        utf8Lookup[ch] = i;
    }
}

void Scribe::BytePairEncoder::train(const std::string& filename, int cycles, bool verbose) {
    WordExtracter extracter;

    std::unordered_map<std::string, Count> strWordCounts = extracter.wordify(filename);
    // TODO: make it so that `strWordCounts` is destroyed after its use

    WordCounts wordCounts;
    wordCounts.reserve(strWordCounts.size());

    for (const auto& [word, count] : strWordCounts) {
        if (word.empty()) continue;

        const std::vector<Token> initialTokens = getCodePoints(word);
        wordCounts.emplace_back(std::move(initialTokens), count);
    }

    for (int i = 0; i < cycles; i++) {
        Pair mostFrequentPair = getMostFrequentPair(wordCounts);
        if (mostFrequentPair == Pair(-1, -1)) {
            std::cerr << "\n\n[INFO]::BYTE_PAIR_ENCODER::CYCLES: Max possible vocab size reached, ";
            std::cerr << "{Cycles: " << cycles << "} is too high for the provided dataset.";
            std::cerr << "\nFinishing gracefully at cycle: " << i << "." << std::endl;
            break;
        }

        int newToken = i + 256;
        doMerge(wordCounts, mostFrequentPair, newToken);

        mergeForest.emplace_back(mostFrequentPair, newToken);

        std::vector<u_int8_t> newBytes = vocab[mostFrequentPair.first];
        newBytes.insert(newBytes.end(), vocab[mostFrequentPair.second].begin(), vocab[mostFrequentPair.second].end());
        vocab[newToken] = newBytes;

        if (!verbose) continue;
        std::clog << "[INFO]::NEW_TOKEN_" << newToken << ": Merged Pair {  " << mostFrequentPair.first;
        std::clog << ", " << mostFrequentPair.second << " \t} : { \'" ;
        for (auto ch : vocab[mostFrequentPair.first]) std::clog << static_cast<char>(ch);
        std::clog << "\'\t, \'";
        for (auto ch : vocab[mostFrequentPair.second]) std::clog << static_cast<char>(ch);
        std::clog << "\' \t}" << std::endl;
    }
}

std::vector<Scribe::Token> Scribe::BytePairEncoder::encode(const std::string& data) {
    std::vector<Token> codepoints = getCodePoints(data);
    WordCounts encodedData = { std::pair<std::vector<Token>, Count>(codepoints, 1) };

    for (const auto& [pair, token] : mergeForest) {
        doMerge(encodedData, pair, token);
    }

    return encodedData[0].first;
}

std::vector<u_int8_t> Scribe::BytePairEncoder::decode(const std::vector<Token>& tokens) {
    std::vector<u_int8_t> decodedData;

    for (const Token token : tokens) {
        decodedData.insert(decodedData.end(), vocab[token].begin(), vocab[token].end());
    }

    return decodedData;
}
