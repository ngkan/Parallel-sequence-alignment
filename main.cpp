#include <iostream>

#include "algorithms.cpp"
#include "utils.cpp"

int main() {
    auto res = RajkoAluru::ConstantGapSolver(
        "abc", "bcd", []() { return 0; }, 1);
    for (auto c : res)
        std::cout << c.first << ' ' << c.second << std::endl;
}
