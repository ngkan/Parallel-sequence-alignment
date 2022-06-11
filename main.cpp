#include <iostream>
#include <string>
#include <filesystem>
#include "algorithms.cpp"
#include "utils.cpp"


void test_nw() {
    std::cout<< "Needleman-Wunsh" << std::endl;

    std::string a = "AAATAGTC";
    std::string b = "TAGTC";

    auto res = NW(a, b, [](char a,char b) { if(a==b) return 1; else return -1; }, -2);
    for (auto c : res)
        std::cout << c.first << ' ' << c.second << std::endl;
}

void test_nw_dw() {
    std::cout<< "Needleman-Wunsh DW" << std::endl;

    std::string a = "AAATAGTC";
    std::string b = "TAGTC";

    auto res = DW_NW(a, b, [](char a,char b) { if(a==b) return 1; else return -1; }, -2);
    for (auto c : res)
        std::cout << c.first << ' ' << c.second << std::endl;
}

void test_gotoh() {
    // std::string a = "ATC";
    // std::string b = "ATCG";
    std::string a = "TAGAAATTT";
    std::string b = "TAGTTT";

    auto res = Gotoh_DW(a, b, [](char x, char y) -> int { return (int) x == y; }, -1, -2);
    for (auto c : res)
        std::cout << c.first << ' ' << c.second << std::endl;

}

void test_nw_bw(){
    std::cout<< "Needleman-Wunsh BW" << std::endl;
    std::string a = "AAATAGTC";
    std::string b = "TAGTC";
    int num_blocks = 2;

    auto res = BW_NW(a, b, [](char a,char b) { if(a==b) return 1; else return -1; }, -2, num_blocks);
    for (auto c : res)
        std::cout << c.first << ' ' << c.second << std::endl;
}

void test_small() {
    const std::string path = "tests/small";

    
}

void test_big() {
    std::string path = "tests/big";

}

int main() {
    test_nw_bw();
    test_nw_dw();
    //test_gotoh();

    // std::cout << fasta::read_single("tests/atf5/Q6P788.fasta");
    // test_small();
    // auto ls = get_tests_from_path("tests/small");
    // for (auto v: ls) {
    //     std::cout << "testname: " << v[0] << '\n' << v[1] << '\n' << v[2] << std::endl;
    // }
}
