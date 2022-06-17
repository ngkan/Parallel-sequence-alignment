#include <iostream>
#include <string>
#include <filesystem>
#include "algorithms.cpp"
#include "utils.cpp"
#include "tests.cpp"

int main() {

    // test_nw_bw("TATC", "AAATATGC", 5, true);
    //stress_tests_gotoh();
    std::cout << "gotoh ok" << std::endl;

    stress_tests_NW();
    std::cout << "NW" << std::endl;
    // std::cout << read_single("tests/atf5/Q6P788.fasta");
    // auto ls = get_tests_from_path("tests/small");
    // for (auto v: ls) {
    //     std::cout << "testname: " << v[0] << '\n' << v[1] << '\n' << v[2] << std::endl;
    //     test_nw_dw(v[1], v[2], 5);
    
    // }

    return 0;
}


