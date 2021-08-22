#include <iostream>
#include <fstream>
#include <filesystem>
#include <stdio.h>
#include <unordered_map>
#include <vector>
#include <ctype.h>

using recursive_directory_iterator =
    std::filesystem::recursive_directory_iterator;
using files = std::unordered_map<std::string,
    std::unordered_map<std::string, size_t>>;

std::unordered_map<std::string, size_t> collect(std::string path) {
    std::ifstream file(path);
    std::unordered_map<std::string, size_t> words;
    std::string s;
    if (file.is_open()) {
        while (file >> s) {
            std::string w;
            for (auto c : s) {
                if (!ispunct(c)) w += c;
                else if (w.size() > 0) {
                    if (words.contains(w)) words[w]++;
                    else words.insert({w, 1});
                    w.erase();
                }
            }
            if (w.size() > 0) {
                if (words.contains(w)) words[w]++;
                else words.insert({w, 1});
            }
        }
        file.close();
    }
    return words;
}

int main(int argc, char** argv) {
    if (argc < 3) std::cerr << "TF-IDF missing arguments" << std::endl;
    files files;
    for (const auto& entry : recursive_directory_iterator(argv[1])) {
        if (!entry.is_directory()) {
            auto words = collect(entry.path());
            if (words.size() > 0) {
                files.insert({entry.path(), words});
            }
            // print words
            std::cout << "Words in file: " << entry.path() << std::endl;
            for (auto word : words) {
                std::cout << word.first << ": " << word.second << std::endl;
            }
        }
    }
}
