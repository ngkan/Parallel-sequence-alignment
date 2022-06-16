#include <chrono>
#include <climits>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "algorithms.cpp"

# define gaps 1
# define match 2
# define mismatch 3

# define max_gap 3

std::pair<std::string, std::string> generate_sequence_pair(int length_sequence){

    std::vector<std::string> nucleo = {"A", "T", "C", "G"};
    std::vector<int> type = {match, match, match, mismatch, gaps};

    std::string A, B;

    for (int i = 0; i<length_sequence; i++){

        int id = (int) rand() % 5; // chooses character
        A += nucleo[id];

        int t = (int) rand() % (type.size()); // chooses B character
        if (t==match){
            B += nucleo[id];
        }
        else if (t==mismatch){
            B += nucleo[(id+1)%5];
        }
        else{
            // pick a gap size
            int siz = (int) rand() % 5; // chooses character
            for (int j=0; j<siz; j++){
                int s = (int) rand() % 5;
                A += nucleo[s];
            }

        }
    }

}




// varying the number of workers

int main(int argc, char* argv[]){

    if (argc < 3) {
        std::cout << "Usage: ./benchmarker num_thread length_sequence" << std::endl;
        return 0;
    }

    int num_threads = std::stoi(argv[1]);
    int num_insertions = std::stoi(argv[2]);

    return 0;
}