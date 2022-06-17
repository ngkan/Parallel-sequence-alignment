#include <iostream>
#include <string>
#include <filesystem>
#include "algorithms.cpp"
#include "utils.cpp"

void test_nw(std::string a, std::string b, bool str_print = false) {
    std::cout<< "\nNeedleman-Wunsh" << std::endl;
    auto res = NW(a, b, [](char a,char b) { if(a==b) return 1; else return -1; }, -2);
    if (str_print)
        print_alignment_letters(res, a, b);
}

void test_nw_dw(std::string a, std::string b, int num_workers, bool str_print = false) {
    std::cout<< "Needleman-Wunsh DW with " << num_workers << " workers" <<std::endl;

    auto res = DW_NW(a, b, [](char a,char b) { if(a==b) return 1; else return -1; },
                     -2, num_workers);

    if (str_print)
        print_alignment_letters(res, a, b);
}

void test_nw_bw(std::string a, std::string b, int num_blocks, bool str_print = false ){
    std::cout<< "Needleman-Wunsh BW with " << num_blocks << " blocks" << std::endl;

    auto res = BW_NW(a, b, [](char a,char b) { if(a==b) return 1; else return -1; },
                     -2, num_blocks);

    if (str_print)
        print_alignment_letters(res, a, b);
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
