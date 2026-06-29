#include "../include/word_extracter.hpp"

#include <set>
#include <fstream>
#include <sstream>
#include <string>

void Scribe::WordExtracter::wordify(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("[ERROR]::COULD_NOT_OPEN_FILE: " + filename);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string filedata(buffer.str());

    file.close();

    const std::set<char> delimiters{'!', '"', '#', '%', '&', '\'', '(', ')', '*', '+', ',',
        '-', '_', '.', '/', ':', ';', '<', '=', '>', '?', '[', ']', '~', '`', '\\', '^', '{', '}', '\n'};

    unsigned long wordStartIndex = 0, wordLen = 0;
    while (wordStartIndex < filedata.length()) {
        const char ch = filedata[wordStartIndex + wordLen];
        
        if (ch == ' ' || ch == '\0') {
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
}
