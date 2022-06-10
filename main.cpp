#include <iostream>
#include <string>

#include "algorithms.cpp"
#include "utils.cpp"

void test_nw() {
    std::string a = "ATC";
    std::string b = "ATCG";

    auto res = Naive::ConstantGapSolver(a, b, []() { return 0; }, 1);
    for (auto c : res)
        std::cout << c.first << ' ' << c.second << std::endl;
}

void test_gotoh() {
    std::string a = "ATC";
    std::string b = "ATCG";

    auto res = Gotoh(a, b, [](char x, char y) -> int { return (int) x == y; }, -1, -2);
    std::cout << "score " << res.first << std::endl;
    for (auto c : res.second)
        std::cout << c.first << ' ' << c.second << std::endl;

}
int main() {
    // test_nw();
    test_gotoh(); 
}
