#include "../include/word_extracter.hpp"

#include <set>
#include <fstream>
#include <sstream>
#include <string>

int Scribe::WordExtracter::getUTFByteLength(const char ch) {
    const unsigned char uc = static_cast<unsigned char>(ch);
    if (uc < 0x80) { return 1; } // 0xxxxxxx, hence a normal ASCII char
    else if ((uc & 0xE0) == 0xC0) { return 2; } // 110xxxxx: 2 bytes
    else if ((uc & 0xF0) == 0xE0) { return 3; } // 1110xxxx: 3 bytes
    else if ((uc & 0xF8) == 0xF0) { return 4; } // 11110xxx: 4 bytes
    return 1; // for invalid utf-8 byte
}

std::unordered_map<std::string, int> Scribe::WordExtracter::wordify(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("[ERROR]::COULD_NOT_OPEN_FILE: " + filename);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string filedata(buffer.str());

    file.close();

    const std::set<char> delimiters{'!', '"', '#', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '_', '.', '/', ':', ';', '<', '=', '>', '?', '[', ']', '~', '`', '\\', '^', '{', '}', '\n'};
        
    std::unordered_map<std::string, int> words;
    unsigned long wordStartIndex = 0, wordLen = 0;

    while (wordStartIndex < filedata.length()) {
        const char ch = filedata[wordStartIndex + wordLen];
        const int utfByteLen = getUTFByteLength(ch);

        if (utfByteLen > 1) {
            // Inserting the word building upto the multibyte character
            words[std::string(&filedata[wordStartIndex], wordLen)]++;
            wordStartIndex += wordLen;

            // Inserting the multibyte character
            words[std::string(&filedata[wordStartIndex], utfByteLen)]++;
            wordStartIndex += utfByteLen;
            wordLen = 0;

        } else if (ch == ' ' || ch == '\0') {
            // Inserting the word building upto the whitespace (or EOF)
            words[std::string(&filedata[wordStartIndex], wordLen)]++;

            wordStartIndex += wordLen;
            wordLen = 1;

            // TODO: Decouple ' ' and '\0' and make individual cases for them; add an EOF token for file-termination.
        } else if (delimiters.count(ch)) {
            // Inserting the word building upto the delimiter
            words[std::string(&filedata[wordStartIndex], wordLen)]++;
            wordStartIndex += wordLen;

            while (delimiters.count(filedata[wordStartIndex])) {
                // Inserting all consecutive (if) occuring delimiters
                words[std::string(1, filedata[wordStartIndex])]++;
                wordStartIndex++;
            }

            wordLen = 1;
        } else {
            wordLen++;
        }

        // TODO: Make the function work with multiple files at once.
    }

    return words;
}
