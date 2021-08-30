#include "Frequency.hpp"
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <iostream>
#include <iomanip>

using recursive_directory_iterator =
    std::filesystem::recursive_directory_iterator;
using file_stats = std::pair<std::unordered_map<std::string, size_t>, size_t>;
using files = std::vector<std::pair<std::string, file_stats>>;

/**
 * Collects all words in the file described by the input path
 * Return a map with key: a word and the value: the n occurence of this word
 * And a size that is the total number of words.
 */
file_stats collect(const std::string& path) {
    std::ifstream file(path);
    std::unordered_map<std::string, size_t> words;
    std::string s;
    size_t total = 0;
    if (file.is_open()) {
        while (file >> s) {
            std::string w;
            for (auto c : s) {
                if (!ispunct(c)) w += c;
                else if (!w.empty()) {
                    total++;
                    if (words.contains(w)) words[w]++;
                    else words.insert({w, 1});
                    w.erase();
                }
            }
            if (!w.empty()) {
                total++;
                if (words.contains(w)) words[w]++;
                else words.insert({w, 1});
            }
        }
        file.close();
    }
    return {words, total};
}

std::thread start_pull(
    std::string &path_folder,
    files &files,
    std::mutex &files_mutex
) {
    return std::thread([path_folder, &files, &files_mutex]() {
        std::queue<std::thread> futures;
        for (const auto& entry : recursive_directory_iterator(path_folder))
            if (!entry.is_directory()) {
                if (futures.size() > 4) {
                    futures.front().join();
                    futures.pop();
                }
                futures.push(std::thread([&files, &files_mutex, entry](){
                    auto words = collect(entry.path().c_str());
                    if (words.second != 0) {
                        std::scoped_lock<std::mutex> lock(files_mutex);
                        files.push_back({entry.path().c_str(), words});
                    }
                }));
            }
        while (!futures.empty()) {
            futures.front().join();
            futures.pop();
        }
    });
}

std::pair<float, float> Frequency::Find(
  std::string directory,
  std::string word
) {
    files files;
    std::mutex files_mutex;
    bool consume = true;
    auto pull = start_pull(directory, files, files_mutex);
    std::pair<float, float> tf_idf;
    std::thread consumer([
        &files,
        &files_mutex,
        &consume,
        word,
        &tf_idf
    ](){
        bool compute = true;
        float total_words = 0.0;
        float total_files = 0.0;
        float count_word = 0.0;
        float count_file = 0.0; // The num of files where it doesn't appear
        while (compute) {
            std::scoped_lock<std::mutex> lock(files_mutex);
            if (files.size() > 0) {
                const auto file = files.back();
                files.pop_back();
                total_words += file.second.second;
                total_files += 1;
                if (file.second.first.contains(word))
                    count_word += file.second.first.at(word);
                else
                    count_file += 1;
            }
            compute = !files.empty() || consume;
        }
        tf_idf.first = count_word / total_words;
        tf_idf.second = count_file / total_files;
    });
    pull.join();
    consume = false;
    consumer.join();
    return tf_idf;
}