#include <iostream>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <tuple>

using recursive_directory_iterator =
    std::filesystem::recursive_directory_iterator;
using files = std::vector<std::tuple<std::string,
    std::unordered_map<std::string, size_t>>>;

/**
 * Collects all words in the file described by the input path
 */
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

std::thread start_pull(
    std::vector<std::string> &paths,
    files &files,
    std::mutex &files_mutex
) {
    return std::thread([&paths, &files, &files_mutex]() {
        std::queue<std::thread> futures;
        for (const auto& path : paths) {
            if (futures.size() > 4) {
                futures.front().join();
                futures.pop();
            }
            futures.push(std::thread([&files, &files_mutex, path](){
                auto words = collect(path);
                if (words.size() > 0) {
                    std::lock_guard<std::mutex> lock(files_mutex);
                    files.push_back({path, words}); // Push first stats here instead of compute it later
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
    if (argc < 2) std::cerr << "TF-IDF missing arguments" << std::endl;
    files files;
    std::mutex files_mutex;
    std::vector<std::string> paths;
    bool consume = true;
    for (const auto& entry : recursive_directory_iterator(argv[1]))
        if (!entry.is_directory())
            paths.push_back(entry.path().c_str());
    auto pull = start_pull(paths, files, files_mutex);
    std::thread consumer([&files, &consume, &files_mutex](){
        bool compute = true;
        while (compute) {
            std::scoped_lock<std::mutex> lock(files_mutex);
            if (!files.empty()) {
                std::cout << std::get<0>(files.back()) << std::endl;
                files.pop_back();
            }
            compute = files.size() > 0 || consume;
        }
    });
    pull.join();
    consume = false;
    consumer.join();
}
