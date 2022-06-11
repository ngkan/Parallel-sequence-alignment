#include <iostream>
#include <string>
#include "algorithms.cpp"
#include "utils.cpp"

void test_nw() {
    std::string a = "ATC";
    std::string b = "ATCG";

    auto res = NW(a, b, [](char a,char b) { if(a==b) return 1; else return -1; }, -2);
    for (auto c : res)
        std::cout << c.first << ' ' << c.second << std::endl;
}

void test_nw_dw() {
    std::string a = "TAGAAATTT";
    std::string b = "TAGTTT";

    auto res = DW_NW(a, b, [](char a,char b) { if(a==b) return 1; else return -1; }, -2);
    for (auto c : res)
        std::cout << c.first << ' ' << c.second << std::endl;
}

void test_gotoh() {
    std::string a = "ATC";
    std::string b = "ATCG";

    auto res = Gotoh(a, b, [](char x, char y) -> int { return (int) x == y; }, -1, -2);
    for (auto c : res)
        std::cout << c.first << ' ' << c.second << std::endl;

}

void test_nw_bw(){

    std::string a = "TAGTAGTAG";
    std::string b = "TAGTAGTAG";
    int num_blocks = 3;

    auto res = BW_NW(a, b, [](char a,char b) { if(a==b) return 1; else return -1; }, -2, num_blocks);
    for (auto c : res)
        std::cout << c.first << ' ' << c.second << std::endl;

}

int main() {
    test_nw_bw();
    //test_nw_dw();
    //test_gotoh(); 
}
