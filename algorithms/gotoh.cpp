#ifndef GOTOH_CPP
#define GOTOH_CPP

#include <atomic>
#include <functional>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "../thread_pool.cpp"

//--------------------------------------single thread version------------------------------------
std::vector<std::pair<int, int>> Gotoh(std::string a, std::string b, std::function<int(char, char)> scoring_function, int gap_penalty, int constant_penalty = 0) {
    // initialize
    size_t n = a.size();
    size_t m = b.size();
    int INF = (int)1e9;

    a = '#' + a;
    b = '#' + b;

    std::vector<std::vector<int>> H(n + 1, std::vector<int>(m + 1, -INF));
    std::vector<std::vector<int>> E(n + 1, std::vector<int>(m + 1, -INF));
    std::vector<std::vector<int>> F(n + 1, std::vector<int>(m + 1, -INF));

    H[0][0] = E[0][0] = F[0][0] = 0;
    for (size_t i = 0; i <= n; i++)
        E[i][0] = constant_penalty + gap_penalty * i;
    for (size_t i = 0; i <= n; i++)
        F[0][i] = constant_penalty + gap_penalty * i;

    // main loop
    for (size_t i = 1; i <= n; i++) {
        for (size_t j = 1; j <= m; j++) {
            F[i][j] = std::max(F[i][j - 1] + gap_penalty, H[i][j - 1] + constant_penalty + gap_penalty);
            E[i][j] = std::max(E[i - 1][j] + gap_penalty, H[i - 1][j] + constant_penalty + gap_penalty);

            H[i][j] = std::max(H[i - 1][j - 1] + scoring_function(a[i], b[j]), std::max(E[i][j], F[i][j]));
        }
    }

    // traceback
    std::vector<std::pair<int, int>> sequence;
    size_t x = n, y = m;
    while (x || y) {
        if (x && y && H[x][y] == H[x - 1][y - 1] + scoring_function(a[x], b[y])) {
            sequence.push_back(std::make_pair(x, y));
            --x;
            --y;
        } else if (H[x][y] == F[x][y]) {
            while (F[x][y] == F[x][y - 1] + gap_penalty) {
                sequence.push_back(std::make_pair(0, y));
                --y;
            }
            sequence.push_back(std::make_pair(0, y));
            --y;
        } else {
            while (E[x][y] == E[x - 1][y] + gap_penalty) {
                sequence.push_back(std::make_pair(x, 0));
                --x;
            }
            sequence.push_back(std::make_pair(x, 0));
            --x;
        }
    }

    std::reverse(sequence.begin(), sequence.end());
    return sequence;
}

//--------------------------------------multi-thread versions------------------------------------

std::vector<std::pair<int, int>> Gotoh_DW(std::string a, std::string b, std::function<int(char, char)> scoring_function,
                                          int gap_penalty, int constant_penalty = 0, int block_size = 2, int nb_threads = 10) {
    // initialize
    ThreadPool tpool(nb_threads);
    size_t n = a.size();
    size_t m = b.size();
    int INF = (int)1e9;

    a = '#' + a;
    b = '#' + b;

    std::vector<std::vector<int>> H(n + 1, std::vector<int>(m + 1, -INF));
    std::vector<std::vector<int>> E(n + 1, std::vector<int>(m + 1, -INF));
    std::vector<std::vector<int>> F(n + 1, std::vector<int>(m + 1, -INF));

    H[0][0] = E[0][0] = F[0][0] = 0;
    for (size_t i = 0; i <= n; i++)
        E[i][0] = constant_penalty + gap_penalty * i;
    for (size_t i = 0; i <= n; i++)
        F[0][i] = constant_penalty + gap_penalty * i;

    // main loop
    auto helper = [](int n, int m, std::string &a, std::string &b, std::vector<std::vector<int>> &H,
                     std::vector<std::vector<int>> &E, std::vector<std::vector<int>> &F,
                     std::function<int(char, char)> scoring_function, int gap_penalty, int constant_penalty,
                     int r1, int r2, int c1, int c2, std::atomic_int32_t &counter) -> void {
        for (size_t i = r1; i <= r2; i++) {
            for (size_t j = c1; j <= c2; j++) {
                F[i][j] = std::max(F[i][j - 1] + gap_penalty, H[i][j - 1] + constant_penalty + gap_penalty);
                E[i][j] = std::max(E[i - 1][j] + gap_penalty, H[i - 1][j] + constant_penalty + gap_penalty);

                H[i][j] = std::max(H[i - 1][j - 1] + scoring_function(a[i], b[j]), std::max(E[i][j], F[i][j]));
            }
        }

        counter--;
    };

    std::atomic_int32_t counter(0);

    for (size_t diag_sum = 0; diag_sum <= n + m; diag_sum += block_size) {
        for (size_t i = 0; i <= std::min(diag_sum, n); i += block_size) {
            size_t j = diag_sum - i;

            if (0 <= i && i < n && 0 <= j && j < m) {
                counter++;

                tpool.push(helper, n, m, std::ref(a), std::ref(b), std::ref(H), std::ref(E), std::ref(F), scoring_function,
                           gap_penalty, constant_penalty, i + 1, std::min(n, i + block_size), j + 1, std::min(m, j + block_size), std::ref(counter));
            }

            while (counter.load() != 0);
        }
    }

    tpool.stop();

    // traceback
    std::vector<std::pair<int, int>> sequence;
    size_t x = n, y = m;
    while (x || y) {
        if (x && y && H[x][y] == H[x - 1][y - 1] + scoring_function(a[x], b[y])) {
            sequence.push_back(std::make_pair(x, y));
            --x;
            --y;
        } else if (H[x][y] == F[x][y]) {
            while (F[x][y] == F[x][y - 1] + gap_penalty) {
                sequence.push_back(std::make_pair(0, y));
                --y;
            }
            sequence.push_back(std::make_pair(0, y));
            --y;
        } else {
            while (E[x][y] == E[x - 1][y] + gap_penalty) {
                sequence.push_back(std::make_pair(x, 0));
                --x;
            }
            sequence.push_back(std::make_pair(x, 0));
            --x;
        }
    }

    std::reverse(sequence.begin(), sequence.end());
    return sequence;
}

#endif
