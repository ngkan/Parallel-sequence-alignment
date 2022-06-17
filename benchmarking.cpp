#include <chrono>
#include <climits>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "algorithms.cpp"
#include "utils.cpp"
#include "tests.cpp"


int main(int argc, char* argv[]){

    if (argc < 3) {
        std::cout << "Usage: ./benchmarker num_thread length_sequence" << std::endl;
        return 0;
    }

    int num_threads = std::stoi(argv[1]);
    int length_sequence = std::stoi(argv[2]);

    stress_tests_gotoh(num_threads, length_sequence);
    // std::cout <<'\n'<< std::endl;
    // stress_tests_NW(length_sequence, num_threads);

    // std::pair<std::string, std::string> AB = generate_sequence_pair(length_sequence);
    
    // test_nw(AB.first, AB.second, true);

    // test_nw_dw(AB.first, AB.second, num_threads, true);

    // test_nw_bw(AB.first, AB.second, num_threads, true);

    return 0;
}