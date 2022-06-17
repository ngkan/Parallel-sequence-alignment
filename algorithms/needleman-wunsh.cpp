#ifndef NAIVE_CPP
#define NAIVE_CPP

#include <algorithm>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <numeric>
#include <queue>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "../thread_pool.cpp"

#define gap_1 1   // (i,j-1) horizontal displacement
#define gap_2 2   // (i-1,j) vertical displacement
#define no_gap 3  // (i-1,j-1) diagonal displacement

//########################### Helper Functions ###############################

void print_2D_array(std::vector<std::vector<int>>* A) {
    int n = (*A).size();     // row number
    int m = (*A)[0].size();  // col number

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            std::cout << (*A)[i][j] << ' ';
        }
        std::cout << std::endl;
    }
}

void Initialization(std::vector<std::vector<int>>* T,
                    std::vector<std::vector<int>>* H, int gap_penalty) {
    int n = (*H).size();     // row number
    int m = (*H)[0].size();  // col number

    int max = std::max(n, m);
    int min = std::min(n, m);

    for (int i = 0; i < min; i++) {
        (*H)[i][0] = i * gap_penalty;
        (*H)[0][i] = i * gap_penalty;
        (*T)[i][0] = gap_2;
        (*T)[0][i] = gap_1;
    }

    if (max == n) {  // extend row axis
        for (int i = min; i < max; i++) {
            (*H)[i][0] = i * gap_penalty;
            (*T)[i][0] = gap_2;
        }
    } else {  // extend column axis
        for (int i = min; i < max; i++) {
            (*H)[0][i] = i * gap_penalty;
            (*T)[0][i] = gap_1;
        }
    }
}

int RecurrenceRelation(std::vector<std::vector<int>>* T, std::vector<std::vector<int>>* H,
                       int i, int j, char a_i, char b_j,
                       std::function<int(char, char)> scoring_function,
                       int gap_penalty) {
    int score;

    int no_gap_sc = (*H)[i - 1][j - 1] + scoring_function(a_i, b_j);
    int gap_1_sc = (*H)[i][j - 1] + gap_penalty;
    int gap_2_sc = (*H)[i - 1][j] + gap_penalty;

    // score comparison and cell indication
    if (no_gap_sc > gap_1_sc) {
        if (no_gap_sc > gap_2_sc) {
            (*T)[i][j] = no_gap;
            score = no_gap_sc;
        } else {
            (*T)[i][j] = gap_2;
            score = gap_2_sc;
        }
    } else {
        if (gap_1_sc > gap_2_sc) {
            (*T)[i][j] = gap_1;
            score = gap_1_sc;
        } else {
            (*T)[i][j] = gap_2;
            score = gap_2_sc;
        }
    }

    return score;
}

std::vector<std::pair<int, int>> Traceback(std::vector<std::vector<int>>* T) {
    std::vector<std::pair<int, int>> res;
    int n = (*T).size();     // row number
    int m = (*T)[0].size();  // col number

    // inner 2d array dimensions
    int row = n - 1;
    int col = m - 1;

    while (true) {  // follow indication matrix for traceback
        int gap_type = (*T)[row][col];

        if (gap_type == gap_2) {
            res.push_back(std::pair(row, 0));
            row -= 1;
        } else if (gap_type == gap_1) {
            res.push_back(std::pair(0, col));
            col -= 1;
        } else if (gap_type == no_gap) {
            res.push_back(std::pair(row, col));
            row -= 1;
            col -= 1;
        }
        if ((row == 0) && (col == 0)) {
            break;
        }
    }

    std::reverse(res.begin(), res.end());

    return res;
}

// ############################## Classes ####################################

// ------------------- Thread pool for Diagonal Wavefront -------------------
class SimplePool {
    unsigned int num_workers;
    std::vector<std::thread> workers;

   public:
    SafeUnboundedQueue<std::pair<std::pair<int, int>, std::pair<int, int>>> tasks;
    std::vector<std::vector<int>>*H, *T;
    std::string a, b;
    std::function<int(char, char)> scoring_function;
    int gap_penalty;
    std::atomic_int32_t counter;

    SimplePool(unsigned int num_workers, std::string a, std::string b,
               std::function<int(char, char)> scoring_function, int gap_penalty) {
        int n = a.length() + 1;
        int m = b.length() + 1;

        std::vector<std::vector<int>>* h = new std::vector<std::vector<int>>(n, std::vector<int>(m, 0));
        std::vector<std::vector<int>>* t = new std::vector<std::vector<int>>(n, std::vector<int>(m, 0));

        Initialization(t, h, gap_penalty);

        this->H = h;
        this->T = t;
        this->scoring_function = scoring_function;
        this->a = a;
        this->b = b;
        this->gap_penalty = gap_penalty;
        this->num_workers = num_workers;

        // send threads to computation loop
        for (int i = 0; i < num_workers; i++) {
            workers.push_back(std::thread(&SimplePool::do_work, this));
        }

        counter = 0;
    }

    ~SimplePool() {
        // join all threads if joinable
        for (int i = 0; i < num_workers; i++) {
            if (workers[i].joinable()) {
                workers[i].join();
            }
        }
    }

    /* thread computation loop */
    void do_work() {
        while (true) {
            std::pair<std::pair<int, int>, std::pair<int, int>> cell = tasks.pop();
            int r1 = cell.first.first;
            int c1 = cell.first.second;
            int r2 = cell.second.first;
            int c2 = cell.second.second;
            // std::cout << "asd " << r1 << ' ' << c1 << ' ' << r2 << ' ' << c2 << std::endl;
            if (r1 == -1) {  // stop taking new tasks
                break;
            }

            for (int i = r1; i <= r2; i++) {
                for (int j = c1; j <= c2; j++) {
                    // std::cout << "ij " << i << ' ' << j << ' ' << a.size() <<' ' << b.size() << std::endl;
                    (*H)[i][j] = RecurrenceRelation(T, H, i, j, a[i - 1], b[j - 1],
                                                    scoring_function, gap_penalty);
                }
            }

            counter--;
            // std::cout << "counter " << counter.load() << std::endl;
        }
    }

    /* stop all computations and wait for threads to finish */
    void stop() {
        // push n stopper tasks
        for (int i = 0; i < num_workers; i++) {
            tasks.push(std::pair(std::pair(-1, -1), std::pair(-1, -1)));
        }

        // join all threads
        for (int i = 0; i < num_workers; i++) {
            workers[i].join();
        }
    }
};

// ------------------ Block class for BLock-Based Wavefront -------------------
class Block {
   public:
    std::vector<std::vector<int>>*H, *T;
    int start, end;
    std::vector<bool>*flag_col, *border_col;
    std::string *a, *b;
    std::function<int(char, char)> scoring_function;
    int gap_penalty, id;

    Block(){};

    Block(int block_id, std::vector<std::vector<int>>* H, std::vector<std::vector<int>>* T,
          int start, int end, std::vector<bool>* flag_col, std::function<int(char, char)> f,
          int p, std::string* a, std::string* b) {
        this->H = H;
        this->T = T;
        this->id = block_id;

        this->start = start;
        this->end = end;

        this->scoring_function = f;
        this->gap_penalty = p;

        this->flag_col = flag_col;
        int n = (*H).size();
        std::vector<bool>* init = new std::vector<bool>(n - 1, false);
        this->border_col = init;
        this->a = a;
        this->b = b;
    }

    /* executes algorithm for block */
    void do_work() {
        int n = (*flag_col).size();

        for (int i = 0; i < n; i++) {
            // wait until computation of previous element
            while (!(*flag_col)[i]) {
                ;
            }
            compute_line(i + 1);
            // mark border element
            (*border_col)[i] = true;
        }
    }

    /* computes cells of i-th line from (i,start) to (i,end) for H and T */
    void compute_line(int i) {
        for (int k = start; k < end; k++) {
            (*H)[i][k] = RecurrenceRelation(T, H, i, k, (*a)[i - 1], (*b)[k - 1],
                                            scoring_function, gap_penalty);
        }
    }
};

// ########################### Algorithms ################################

//---------------------- Needleman-Wunsh (NW) ---------------------------
std::pair<int, std::vector<std::pair<int, int>>> NW(std::string a, std::string b,
                                                    std::function<int(char, char)> scoring_function,
                                                    int gap_penalty) {
    int n = a.length() + 1;  // row number
    int m = b.length() + 1;  // col number

    std::vector<std::vector<int>>* H = new std::vector<std::vector<int>>(n, std::vector<int>(m, 0));
    std::vector<std::vector<int>>* T = new std::vector<std::vector<int>>(n, std::vector<int>(m, 0));

    // ---------------- Initialization --------------

    Initialization(T, H, gap_penalty);

    // ------------------ Algorithm  ------------------

    for (int i = 1; i < n; i++) {
        for (int j = 1; j < m; j++) {
            (*H)[i][j] = RecurrenceRelation(T, H, i, j, a[i - 1], b[j - 1],
                                            scoring_function, gap_penalty);
        }
    }

    // print_2D_array(T);
    // print_2D_array(H);

    int score = (*H)[n - 1][m - 1];
    return std::pair<int, std::vector<std::pair<int, int>>>(score, Traceback(T));
}

//---------------- Diagonal Wavefront (DW) applied to NW ----------------
std::pair<int, std::vector<std::pair<int, int>>> DW_NW(std::string a, std::string b,
                                                       std::function<int(char, char)> scoring_function,
                                                       int gap_penalty, int num_workers, int block_size) {
    int n = a.length() + 1;  // row number
    int m = b.length() + 1;  // col number

    // ---------------- Initialization --------------
    SimplePool worker_pool = SimplePool(num_workers, a, b, scoring_function, gap_penalty);

    // // ------------------ Algorithm  ------------------
    // int r, c;
    // // -------------- row axis antidiagonals ----------
    // for (int i = 1; i < n; i++) {
    //     r = i;
    //     c = 1;
    //     worker_pool.counter = 0;
    //     // construct anti-diagonal and push tasks
    //     while ((1 <= r) && (c < m)) {
    //         worker_pool.counter++;
    //         worker_pool.tasks.push(std::pair(r, c));
    //         r -= 1;
    //         c += 1;
    //     }
    //     // wait until computations are finished
    //     while (worker_pool.counter.load()!=0) {
    //         ;
    //     }
    // }
    // // -------------- column axis antidiagonals ---------
    // for (int j = 2; j < m; j++) {
    //     r = n - 1;
    //     c = j;
    //     worker_pool.counter = 0;
    //     // construct anti-diagonal and push tasks
    //     while ((1 <= r) && (c < m)) {
    //         worker_pool.counter++;
    //         worker_pool.tasks.push(std::pair(r, c));
    //         r -= 1;
    //         c += 1;
    //     }
    //     // wait until computations are finished
    //     while (worker_pool.counter.load()!=0) {
    //         ;
    //     }
    // }

    for (int diag_sum = 0; diag_sum <= (n-1) + (m-1); diag_sum += block_size) {
        worker_pool.counter = 0;
        for (int i = std::min(diag_sum, (n-1) / block_size * block_size); i >= 0 && diag_sum - i <= (m-1); i -= block_size) {
            int j = diag_sum - i;

            worker_pool.counter++;
            worker_pool.tasks.push(std::pair(std::pair(i+1, j+1), std::pair(std::min(n-1, i + block_size), std::min(m-1, j + block_size))));

            if (i == 0)
                break;
        }
        while (worker_pool.counter.load() != 0);
    }

    worker_pool.stop();  // join all threads

    int score = (*worker_pool.H)[n - 1][m - 1];

    return std::pair<int, std::vector<std::pair<int, int>>>(score, Traceback(worker_pool.T));
}

//-------------- Block-based Wavefront (BW) applied to NW ----------------
std::pair<int, std::vector<std::pair<int, int>>> BW_NW(std::string a, std::string b,
                                                       std::function<int(char, char)> scoring_function,
                                                       int gap_penalty, int num_blocks) {
    int n = a.length() + 1;  // row number
    int m = b.length() + 1;  // col number

    std::vector<std::vector<int>>* H = new std::vector<std::vector<int>>(n, std::vector<int>(m, 0));
    std::vector<std::vector<int>>* T = new std::vector<std::vector<int>>(n, std::vector<int>(m, 0));

    // ---------------- Initialization --------------
    Initialization(T, H, gap_penalty);

    // -------------- Block Creation ----------------
    std::vector<Block> blocks(num_blocks);

    if (num_blocks == 1) {
        std::vector<bool>* flag = new std::vector<bool>(n - 1, true);
        blocks[0] = Block(0, H, T, 1, m, flag,
                          scoring_function, gap_penalty, &a, &b);

    }

    else {
        // --- block size determination ----
        int block_size = (int)(m - 1) / num_blocks;
        int last_size = block_size + ((m - 1) % num_blocks);

        int start = 1;
        int end = start + block_size;
        std::vector<bool>* flag = new std::vector<bool>(n - 1, true);

        // --- first block ----
        blocks[0] = Block(0, H, T, start, end, flag,
                          scoring_function, gap_penalty, &a, &b);

        // --- next blocks ----
        for (int i = 1; i < (num_blocks - 1); i++) {
            start += block_size;
            end += block_size;
            blocks[i] = Block(i, H, T, start, end, blocks[i - 1].border_col,
                              scoring_function, gap_penalty, &a, &b);
        }

        // --- last block ---
        int l = num_blocks - 1;
        start += block_size;
        end += last_size;

        blocks[l] = Block(l, H, T, start, end, blocks[l - 1].border_col,
                          scoring_function, gap_penalty, &a, &b);
    }

    // ------------------ Algorithm  ------------------
    std::vector<std::thread> workers(num_blocks);

    for (int i = 0; i < num_blocks; i++) {
        workers[i] = std::thread(&Block::do_work, blocks[i]);
    }

    // ------------------ Joining ---------------------
    for (int i = 0; i < num_blocks; i++) {
        workers[i].join();
    }

    int score = (*H)[n - 1][m - 1];
    return std::pair<int, std::vector<std::pair<int, int>>>(score, Traceback(T));
}

#endif
