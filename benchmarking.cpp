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

# define gaps 1
# define match 2
# define mismatch 3

# define max_gap 3

std::pair<std::string, std::string> generate_sequence_pair(int length_sequence){

    std::vector<std::string> nucleo;
    nucleo.push_back("T");
    nucleo.push_back("A");
    nucleo.push_back("G");
    nucleo.push_back("C");

    std::vector<int> type;
    type.push_back(match);
    type.push_back(match);
    type.push_back(mismatch);
    type.push_back(gaps);

    std::string A, B;

    for (int i = 0; i<length_sequence; i++){

        int id = (int) rand() % 4; // chooses character
        std::cout << "id" << id << std::endl;
        A += nucleo[id];

        int t = (int) rand() % (type.size()); // chooses B character
        if (t==match){
            B += nucleo[id];
        }
        else if (t==mismatch){
            B += nucleo[(id+1)%4];
        }
        else{
            // pick a gap size
            int seq = (int) rand() % 2; // chooses sequence
            int siz = (int) rand() % max_gap; // chooses gap size
            std::cout << "seq" << seq << std::endl;
            if (seq==1){ // add characters to A
                for (int j=0; j<siz; j++){
                    int s = (int) rand() % 4;
                    A += nucleo[s];
                }
            }

            else{ // add characters to B
                for (int j=0; j<siz; j++){
                    int s = (int) rand() % 4;
                    B += nucleo[s];
                }
            }
        }
    }

    return std::pair<std::string, std::string>(A,B);

}

// void test_nw(std::string a, std::string b) {
//     std::cout<< "\nNeedleman-Wunsh" << std::endl;
//     auto res = NW(a, b, [](char a,char b) { if(a==b) return 1; else return -1; }, -2);

//     print_alignment_letters(res, a, b);
// }


// varying the number of workers

int main(int argc, char* argv[]){

    if (argc < 3) {
        std::cout << "Usage: ./benchmarker num_thread length_sequence" << std::endl;
        return 0;
    }

    int num_threads = std::stoi(argv[1]);
    int length_sequence = std::stoi(argv[2]);

    std::pair<std::string, std::string> AB = generate_sequence_pair(length_sequence);
    
    test_nw(AB.first, AB.second, true);

    test_nw_dw(AB.first, AB.second, 5, true);

    test_nw_bw(AB.first, AB.second, 5, true);

    return 0;
}