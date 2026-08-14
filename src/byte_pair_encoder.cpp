#include "../include/byte_pair_encoder.hpp"

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <cstdint>
#include <fmt/format.h>

#include <../include/typedefs.hpp>
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
            pairCounts[pair] += count;

            if (pairCounts[pair] > maxCount) {
                maxCount = pairCounts[pair];
                mostFrequentPair = pair;
            }

            i++; j++;
        }
    }

    return mostFrequentPair;
}

Scribe::Pair Scribe::BytePairEncoder::getBestRankedPair(const WordCounts& wordCounts) {
    PairCounts pairCounts;
    std::unordered_map<Token, Count> tokenCounts;
    double maxScore = 0;
    Pair bestPair = { -1, -1 };

    for (const auto& [word, count] : wordCounts) {
        int len = word.size();
        if (len < 2) continue;

        int i = 0, j = 1;
        tokenCounts[word[0]] += count;
        while (j < len) {
            Pair pair = { word[i], word[j] };
            pairCounts[pair] += count;
            tokenCounts[word[j]] += count;

            i++; j++;
        }
    }

    for (const auto& [word, count] : wordCounts) {
        int len = word.size();
        if (len < 2) continue;

        int i = 0, j = 1;
        while (j < len) {
            Pair pair = { word[i], word[j] };

            double score = (double)pairCounts[pair] / tokenCounts[word[i]];
            score /= tokenCounts[word[j]];
            if (score > maxScore) {
                maxScore = score;
                bestPair = pair;
            }

            i++; j++;
        }
    }

    return bestPair;
}

void Scribe::BytePairEncoder::doMerge(WordCounts& wordCounts, const Pair& pairToMerge, const int newToken) {
    for (auto& [word, _] : wordCounts) {
        int len = word.size();
        int read_idx = 0;
        int write_idx = 0;

        while (read_idx < len) {
            if (read_idx + 1 < len && word[read_idx] == pairToMerge.first && word[read_idx + 1] == pairToMerge.second) {
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
        const uint8_t ch = static_cast<uint8_t>(i);

        vocab[i] = { ch };
        utf8Lookup[ch] = i;
    }
}

void Scribe::BytePairEncoder::train(const std::string& filename, int cycles, bool normalizedRanking, bool verbose) {
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

    Pair pairToMerge;
    for (int i = 0; i < cycles; i++) {
        if (!normalizedRanking) pairToMerge = getMostFrequentPair(wordCounts);
        else pairToMerge = getBestRankedPair(wordCounts);

        if (pairToMerge == Pair(-1, -1)) {
            std::cerr << "\n\n[INFO]::BYTE_PAIR_ENCODER::CYCLES: Max possible vocab size reached, ";
            std::cerr << "{Cycles: " << cycles << "} is too high for the provided dataset.";
            std::cerr << "\nFinishing gracefully at cycle: " << i << "." << std::endl;
            break;
        }

        int newToken = i + 256;
        doMerge(wordCounts, pairToMerge, newToken);

        mergeForest.emplace_back(pairToMerge, newToken);

        std::vector<uint8_t> newBytes = vocab[pairToMerge.first];
        newBytes.insert(newBytes.end(), vocab[pairToMerge.second].begin(), vocab[pairToMerge.second].end());
        vocab[newToken] = newBytes;

        if (!verbose) continue;
        std::string w1 = "", w2 = "";
        for (auto ch : vocab[pairToMerge.first]) w1 += static_cast<char>(ch);
        for (auto ch : vocab[pairToMerge.second]) w2 += static_cast<char>(ch);
        std::string w3 = w1 + w2;
        std::clog << fmt::format("[INFO]::NEW_TOKEN_{:<6}: Merged Pair {{{:^6}, {:^6}}} : {{{:^15}, {:^15}}} -> {:<40}", newToken, pairToMerge.first, pairToMerge.second, w1, w2, w3) << std::endl;
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

std::vector<uint8_t> Scribe::BytePairEncoder::decode(const std::vector<Token>& tokens) {
    std::vector<uint8_t> decodedData;

    for (const Token token : tokens) {
        decodedData.insert(decodedData.end(), vocab[token].begin(), vocab[token].end());
    }

    return decodedData;
}
