#include <iostream>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>

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
    char* path_folder,
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
                    //std::cout << "Collect this " << path << std::endl;
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

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "TF-IDF missing arguments" << std::endl;
        return 1;
    }
    files files;
    std::mutex files_mutex;
    bool consume = true;
    auto pull = start_pull(argv[1], files, files_mutex);
    std::string word(argv[2]);

    std::thread consumer([&files, &consume, &files_mutex, word](){
        bool compute = true;
        while (compute) {
            std::scoped_lock<std::mutex> lock(files_mutex);
            if (files.size() > 0) {
                const auto file = files.back();
                std::cout << "Read " << file.first;
                files.pop_back();
                if (file.second.first.contains(word))
                    std::cout << " Frenquency: "
                    << ((float)file.second.first.at(word) / file.second.second)
                    << std::endl;
                else std::cout << " Frenquency: 0" << std::endl;
            }
            compute = !files.empty() || consume;
        }
    });
    pull.join();
    consume = false;
    consumer.join();
}
