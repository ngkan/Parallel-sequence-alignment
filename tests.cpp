#include <filesystem>
#include <iostream>
#include <string>

#include "algorithms.cpp"
#include "utils.cpp"

void test_nw(std::string a, std::string b, bool str_print = false) {
    std::cout << "\nNeedleman-Wunsh" << std::endl;
    auto res = NW(
        a, b, [](char a, char b) { if(a==b) return 1; else return -1; }, -2);
    if (str_print)
        print_alignment_letters(res.second, a, b);
    std::cout << "Final score: " << res.first << std::endl;
}

void test_nw_dw(std::string a, std::string b, int num_workers, bool str_print = false) {
    std::cout << "Needleman-Wunsh DW with " << num_workers << " workers" << std::endl;

    auto res = DW_NW(
        a, b, [](char a, char b) { if(a==b) return 1; else return -1; },
        -2, num_workers, 100);

    if (str_print)
        print_alignment_letters(res.second, a, b);
    std::cout << "Final score: " << res.first << std::endl;
}

void test_nw_bw(std::string a, std::string b, int num_blocks, bool str_print = false) {
    std::cout << "Needleman-Wunsh BW with " << num_blocks << " blocks" << std::endl;

    auto res = BW_NW(
        a, b, [](char a, char b) { if(a==b) return 1; else return -1; },
        -2, num_blocks);

    if (str_print)
        print_alignment_letters(res.second, a, b);
    std::cout << "Final score: " << res.first << std::endl;
}

void test_gotoh() {
    // std::string a = "ATC";
    // std::string b = "ATCG";

    std::string a = "TAGAAATTT";
    std::string b = "TAGTTT";

    auto [score, alignment] = Gotoh_DW(
        a, b, [](char x, char y) -> int { return (int)x == y; }, -1, -2);
    std::cout << "score " << score << std::endl;
    print_alignment_letters(alignment, a, b);

    // auto res = Gotoh_DW(a, b, [](char x, char y) -> int { return (int) x == y; }, -1, -2);
    // for (auto c : res)
    //     std::cout << c.first << ' ' << c.second << std::endl;
}

void stress_tests_gotoh() {
    int nb_tests = 1;
    int length = 5000;

    auto sfunc = [](char x, char y) -> int { return (int)x == y; };
    auto gap_pen = 0;
    auto const_pen = 0;

    while (nb_tests--) {
        auto [a, b] = generate_sequence_pair(length);
        
        auto start = std::chrono::system_clock::now();
        auto [score, alignment] = Gotoh(a, b, sfunc, gap_pen, const_pen);
        auto end = std::chrono::system_clock::now();
        std::cout << "run gotoh naive time (in ms): " << std::chrono::duration<double, std::milli>(end - start).count() << std::endl;

        auto start_dw = std::chrono::system_clock::now();
        auto [score_dw, alignment_dw] = Gotoh_DW(a, b, sfunc, gap_pen, const_pen, 200, 10);
        auto end_dw = std::chrono::system_clock::now();
        std::cout << "run gotoh dw time (in ms): " << std::chrono::duration<double, std::milli>(end_dw - start_dw).count() << std::endl;


        auto start_bw = std::chrono::system_clock::now();
        auto [score_bw, alignment_bw] = Gotoh_BW(a, b, sfunc, gap_pen, const_pen, 10);
        auto end_bw = std::chrono::system_clock::now();
        std::cout << "run gotoh bw time (in ms): " << std::chrono::duration<double, std::milli>(end_bw - start_bw).count() << std::endl;

        if (score != score_dw || score != score_bw) {
            print_alignment_letters(alignment, a, b);
            print_alignment_letters(alignment_dw, a, b);
            print_alignment_letters(alignment_bw, a, b);
            std::cout << "oops" << std::endl;
        }
    }
}

void stress_tests_NW() {
    int nb_tests = 1;
    int length = 5000;

    auto sfunc = [](char a, char b) { if(a==b) return 1; else return -1; };
    int gap_pen = -2;
    // std::vector<int> num_thr = {1, 2, 3, 4, 5};
    std::vector<int> num_thr = {10};

    while (nb_tests--) {
        int id = rand() % num_thr.size();
        auto [a, b] = generate_sequence_pair(length);

        auto start = std::chrono::system_clock::now();
        auto [score, alignment] = NW(a, b, sfunc, gap_pen);
        auto end = std::chrono::system_clock::now();
        std::cout << "run nw naive time (in ms): " << std::chrono::duration<double, std::milli>(end - start).count() << std::endl;

        auto start_dw = std::chrono::system_clock::now();
        auto [score_dw, alignment_dw] = DW_NW(a, b,sfunc, gap_pen, num_thr[id], 200);
        auto end_dw = std::chrono::system_clock::now();
        std::cout << "run nw dw time (in ms): " << std::chrono::duration<double, std::milli>(end_dw - start_dw).count() << std::endl;


        auto start_bw = std::chrono::system_clock::now();
        auto [score_bw, alignment_bw] = BW_NW(a, b,sfunc, gap_pen, num_thr[id]);
        auto end_bw = std::chrono::system_clock::now();
        std::cout << "run nw bw time (in ms): " << std::chrono::duration<double, std::milli>(end_bw - start_bw).count() << std::endl;
        if (score != score_dw) {
            print_alignment_letters(alignment, a, b);
            print_alignment_letters(alignment_dw, a, b);
            std::cout << "oops DW not equal to NW" << std::endl;
        }
        if (score != score_bw) {
            print_alignment_letters(alignment, a, b);
            print_alignment_letters(alignment_bw, a, b);
            std::cout << "oops BW not equal to NW" << std::endl;
        }
    }
}