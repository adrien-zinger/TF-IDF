#include "Frequency.hpp"
#include <iostream>
#include <iomanip>

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "TF-IDF missing arguments" << std::endl;
        return 1;
    }
    Frequency freq;
    const auto tf_idf = freq.Find(argv[1], argv[2]);
    std::cout << std::fixed << std::setprecision(3)
    << "Term frequency: " << (tf_idf.first * 100) << "%" << std::endl
    << "Inverse document frequency: " << (tf_idf.second * 100) << "%" << std::endl;
}
