#ifndef UTILS_CPP
#define UTILS_CPP

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

// read a whole file or fasta files with only one sequence
std::string read_single(std::filesystem::path file, bool is_fasta = true) {
    std::ifstream filestream(file);
    std::string line;
    std::string res;

    bool firstline = is_fasta;
    while (std::getline(filestream, line)) {
        if (firstline) {
            firstline = false;
            continue;
        }
        res += line;
    }

    return res;
}

// read regular fasta files
std::map<std::string, std::string> read_fasta(std::filesystem::path file) {
    return {};
}

// print valid test folders in given directory
std::vector<std::vector<std::string>> get_tests_from_path(std::filesystem::path path) {
    std::vector<std::vector<std::string>> result;

    for (auto const& dir_entry : std::filesystem::directory_iterator{path}) {
        if (not std::filesystem::is_directory(dir_entry)) {
            std::cerr << dir_entry.path() << " is not a dictionary; skipped." << std::endl;
            continue;
        }

        size_t count = 0;
        bool valid = true;
        std::vector<std::string> sequences;

        for (auto const& subfile : std::filesystem::directory_iterator{dir_entry}) {
            count++;

            if (count > 2) {
                valid = false;
                break;
            }
            if (not std::filesystem::is_regular_file(subfile)) {
                std::cout << "oops\n";
                valid = false;
            }

            sequences.push_back(read_single(subfile.path(), subfile.path().extension() == ".fasta"));
        }

        if (count != 2 || not valid) {
            std::cerr << dir_entry.path() << " is not a valid test folder; skipped." << std::endl;
            continue;
        }

        std::string test_name = dir_entry.path().filename();
        sequences.insert(sequences.begin(), test_name);
        result.push_back(sequences);
    }

    return result;
}

// changes alignment format from a pair vector format to a letter format
void print_alignment_letters(std::vector<std::pair<int, int>> C, std::string a, std::string b) {
    for (auto c : C) {
        char fir;
        if (c.first == 0) {
            fir = '-';
        } else {
            fir = a[c.first - 1];
        }
        std::cout << fir << ' ';
    }
    std::cout << std::endl;

    for (auto c : C) {
        char sec;
        if (c.second == 0) {
            sec = '-';
        } else {
            sec = b[c.second - 1];
        }
        std::cout << sec << ' ';
    }
    std::cout << std::endl;
}

#define gaps 1
#define match 2
#define mismatch 3
#define max_gap 3
std::pair<std::string, std::string> generate_sequence_pair(int length_sequence) {
    std::vector<char> nucleo = {'A', 'T', 'G', 'C'};
    std::vector<int> type = {match, match, mismatch, gaps};
    std::string A, B;

    for (int i = 0; i < length_sequence; i++) {
        // choose A character
        int tA = (int)rand() % 4;
        A += nucleo[tA];

        int tB = (int)rand() % (type.size());
        if (type[tB] == match)
            B += nucleo[tB];
        else if (type[tB] == mismatch)
            B += nucleo[(tA + 1) % 4];
        else {
            // pick a gap size
            int seq = (int)rand() % 2;        // chooses sequence
            int siz = (int)rand() % max_gap;  // chooses gap size

            if (seq == 1) {  // add characters to A
                for (int j = 0; j < siz; j++)
                    A += nucleo[(int)rand() % 4];
            } else {  // add characters to B
                for (int j = 0; j < siz; j++)
                    B += nucleo[(int)rand() % 4];
            }
        }
    }

    return std::pair<std::string, std::string>(A, B);
}
#undef gaps
#undef match
#undef mismatch
#undef max_gap

#endif
