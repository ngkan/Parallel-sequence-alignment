#include <iostream>
#include <string>

#include "algorithms.cpp"
#include "utils.cpp"

void test_nw() {
    std::string a = "ATC";
    std::string b = "ATCG";

    //auto func = [](char a,char b) { if(a==b) return 1; else return -1; };

    auto res = NW(a, b, [](char a,char b) { if(a==b) return 1; else return -1; }, -2);
    for (auto c : res)
        std::cout << c.first << ' ' << c.second << std::endl;
}

void test_gotoh() {
    std::string a = "ATC";
    std::string b = "ATCG";

    auto res = Gotoh(a, b, [](char x, char y) { return x == y; }, -1, -2);
    for (auto c : res.second)
        std::cout << c.first << ' ' << c.second << std::endl;

}
int main() {
    test_nw();
    //test_gotoh(); 
}
