#ifndef NAIVE_CPP
#define NAIVE_CPP

#include <functional>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <future>
#include <iostream>
#include <cmath>
#include <mutex>
#include <numeric>
#include <thread>
#include <queue>

#include "../thread_pool.cpp"

#define gap_1 1 // (i,j-1)
#define gap_2 2 // (i-1,j)
#define no_gap 3


int RecurrenceRelation(std::vector<std::vector<int>>& T,std::vector<std::vector<int>>& H, int i,int j,char a_i, char b_j, std::function<int(char, char)> scoring_function, int gap_penalty){

    int score; 

    int no_gap_sc = H[i-1][j-1] + scoring_function(a_i,b_j);
    int gap_1_sc = H[i][j-1] + gap_penalty;
    int gap_2_sc = H[i-1][j] + gap_penalty;

    // score comparison and cell indication
    if (no_gap_sc > gap_1_sc){
        if (no_gap_sc > gap_2_sc){
            T[i][j] = no_gap;
            score = no_gap_sc;
        }
        else{
            T[i][j] = gap_2;
            score = gap_2_sc;
        }
    }
    else{
        if (gap_1_sc > gap_2_sc){
            T[i][j] = gap_1;
            score = gap_1_sc;
        }
        else{
            T[i][j] = gap_2;
            score = gap_2_sc;
        }
    }

    return score;
}


std::vector<std::pair<int, int>> NW(std::string a, std::string b, std::function<int(char, char)> scoring_function, int gap_penalty) {
    
    int n = a.length()+1; // row number
    int m = b.length()+1; // col number

    std::vector<std::vector<int>> H(n, std::vector<int> (m)), T(n, std::vector<int> (m,0));

    // H DP matrix of score
    // T traceback matrix

    int max = std::max(n,m);
    int min = std::min(n,m);

    // ---------------- Initialization --------------

    for (int i = 0; i<min; i++){
        H[i][0] = i*gap_penalty;
        H[0][i] = i*gap_penalty;
    }

    if (max==n){  // extend row axis
        for (int i = min; i<max; i++ ){
            H[i][0] = i*gap_penalty;
        }
    }
    else{ // extend column axis
        for (int i = min; i<max; i++ ){
            H[0][i] = i*gap_penalty;
        }
    }

    // ------------------ Algorithm  ------------------

    for (int i = 1; i<n; i++){
        for (int j = 1; j<m; j++){
            H[i][j] = RecurrenceRelation(T,H,i,j,a[i-1],b[j-1],scoring_function, gap_penalty);
        }
    }

    // // ----------------- print T ---------------------

    // for (int i = 0; i < n; ++i){
    //     for (int j = 0; j < m; ++j){
    //         std::cout << T[i][j] << ' ';
    //     }
    //     std::cout << std::endl;
    // }

    // // ---------------- print H --------------------

    // for (int i = 0; i < n; ++i){
    //     for (int j = 0; j < m; ++j){
    //         std::cout << ' ' << ' ' << H[i][j] << ' ' << ' ';
    //     }
    //     std::cout << std::endl;
    // }          

    // ----------------- Traceback --------------------
    
    std::vector<std::pair<int, int>> res;

    int row = n-1;
    int col = m-1;

    while (true){ // follow indication matrix for traceback
        int gap_type = T[row][col];

        if (gap_type==gap_2){
            res.push_back(std::pair(row,0));
            row -= 1;
        }
        else if (gap_type==gap_1){
            res.push_back(std::pair(0,col));
            col -= 1;
        }
        else if (gap_type==no_gap){
            res.push_back(std::pair(row,col));
            row -= 1;
            col -= 1;
        }
        if ((row==0)&&(col==0)){
            break;
        }
    }

    std::reverse(res.begin(),res.end());

    return res;
}


//---------------------- Thread pool for Diagonal Wavefront --------------------------------

class SimplePool {
        unsigned int num_workers;
        std::vector<std::thread> workers;
        void do_work();

    public:
        SafeUnboundedQueue< std::pair<int,int> > tasks;
        SimplePool(unsigned int num_workers, std::string a, std::string b, std::function<int(char, char)> scoring_function, int gap_penalty);
        ~SimplePool();
        std::vector<std::vector<int>> H,T;
        void push(std::pair<int, int> cell);
        void stop();
        std::string a, b;
        std::function<int(char, char)> scoring_function;
        int gap_penalty;
};

void SimplePool::do_work() {

    while(true){
        std::pair<int, int> cell = tasks.pop();
        int i = cell.first;
        int j = cell.second;

        if (i==-1){ // stopper task
            break; // stop taking new tasks
        }

        H[i][j] = RecurrenceRelation(T,H, i,j,a[i-1], b[j-1], scoring_function, gap_penalty);

    }

}

SimplePool::SimplePool(unsigned int num_workers, std::string a, std::string b, std::function<int(char, char)> scoring_function, int gap_penalty) {

    int n = a.length()+1;
    int m = b.length()+1;

    int max = std::max(n,m);
    int min = std::min(n,m);

    std::vector<std::vector<int>> h(n, std::vector<int> (m));
    std::vector<std::vector<int>> t(n, std::vector<int> (m,0));
    
    for (int i = 0; i<min; i++){
        h[i][0] = i*gap_penalty;
        h[0][i] = i*gap_penalty;
    }

    if (max==n){  // extend row axis
        for (int i = min; i<max; i++ ){
            h[i][0] = i*gap_penalty;
        }
    }
    else{ // extend column axis
        for (int i = min; i<max; i++ ){
            h[0][i] = i*gap_penalty;
        }
    }

    this->H = h;
    this->T = t;
    this->scoring_function = scoring_function;
    this->a = a;
    this->b = b;
    this->gap_penalty = gap_penalty;
    this->num_workers = num_workers;

    // send threads to computation loop
    for (int i = 0; i<num_workers; i++){
        workers.push_back(std::thread(&SimplePool::do_work, this));
    }
}

SimplePool::~SimplePool() {
    // join all threads if joinable
    for (int i = 0; i<num_workers; i++){
        if (workers[i].joinable()){
            workers[i].join();
        }
    }

}

void SimplePool::push(std::pair<int, int> cell) {

    tasks.push(cell);

}

void SimplePool::stop() {

    // push n stopper tasks
    for (int i = 0; i<num_workers; i++){
        tasks.push(std::pair(-1,-1)); 
    }

    // join all threads
    for (int i = 0; i<num_workers; i++){
        workers[i].join();
    }
}

//---------------------- Diagonal Wavefront (DW) applied to Needleman-Wunsh ------------------------------

std::vector<std::pair<int, int>> DW_NW(std::string a, std::string b, std::function<int(char, char)> scoring_function, int gap_penalty) {
    
    int n = a.length()+1; // row number
    int m = b.length()+1; // col number

    // ---------------- Initialization --------------

    unsigned int num_workers = std::min(n,m)-1; // maximal antidiagonal size for inner matrix (without first row and first column)
    SimplePool worker_pool= SimplePool(num_workers, a, b, scoring_function, gap_penalty );

    // ------------------ Algorithm  ------------------

    int r, c;

    // iterate over antidiagonals on the row axis
    for (int i = 1; i<n; i++){
        r = i;
        c = 1;
        // construct anti-diagonal and push tasks
        while ((1<=r)&&(c<m)){
            worker_pool.push(std::pair(r,c));
            r -= 1;
            c += 1;
        }
        // wait until computations are finished
        while(!worker_pool.tasks.is_empty()){ 
            ;
        }
    }
    // iterate over antidiagonals on the column axis
    for (int j = 2; j<m; j++){
        r = n-1;
        c = j;
        // construct anti-diagonal and push tasks
        while ((1<=r)&&(c<m)){
            worker_pool.push(std::pair(r,c));
            r -= 1;
            c += 1;
        }
        // wait until computations are finished
        while(!worker_pool.tasks.is_empty()){
            ;
        }
    }

    worker_pool.stop(); // join all threads

    std::vector<std::vector<int>> H,T;
    H = worker_pool.H;
    T = worker_pool.T;

    // ----------------- print T ---------------------

    // for (int i = 0; i < n; ++i){
    //     for (int j = 0; j < m; ++j){
    //         std::cout << T[i][j] << ' ';
    //     }
    //     std::cout << std::endl;
    // }

    //  ---------------- print H --------------------

    // for (int i = 0; i < n; ++i){
    //     for (int j = 0; j < m; ++j){
    //         std::cout << ' ' << ' ' << H[i][j] << ' ' << ' ';
    //     }
    //     std::cout << std::endl;
    // }          

    // ----------------- Traceback --------------------
    
    std::vector<std::pair<int, int>> res;

    int row = n-1;
    int col = m-1;

    while (true){ // follow indication matrix for traceback
        int gap_type = T[row][col];

        if (gap_type==gap_2){
            res.push_back(std::pair(row,0));
            row -= 1;
        }
        else if (gap_type==gap_1){
            res.push_back(std::pair(0,col));
            col -= 1;
        }
        else if (gap_type==no_gap){
            res.push_back(std::pair(row,col));
            row -= 1;
            col -= 1;
        }
        if ((row==0)&&(col==0)){
            break;
        }
    }

    std::reverse(res.begin(),res.end());

    return res;
}

// ---------------------------- Block class for BLock-Based Wavefront -------------------------------

class Block{
    public:
        std::vector<std::vector<int>> H,T;
        int start, end;
        std::vector<bool> start_col, border_col;
    
    Block(std::vector<std::vector<int>>& H,std::vector<std::vector<int>>& T, int start, int end, std::vector<bool>& start_col){
        this->H = H;
        this->T = T;
        this->start = start;
        this-> end = end;
        this->start_col = start_col;
        int n = H.size();
        std::vector<bool> init(n-1,false);
        this->border_col = init;
    }

    void do_work(){

        int i = 0;
        int n = H.size();

    }

};

//---------------------- BLock-Based Wavefront (BW) applied to Needleman-Wunsh ------------------------------

std::vector<std::pair<int, int>> BW_NW(std::string a, std::string b, std::function<int(char, char)> scoring_function, int gap_penalty, int num_blocks) {
    
    int n = a.length()+1; // row number
    int m = b.length()+1; // col number

    std::vector<std::vector<int>> H,T;

    // ---------------- Initialization --------------

    int block_size = (int) (m-1)/n;
    int last_size = (m-1) % n;

    // ------------------ Algorithm  ------------------

    int r, c;

    // iterate over antidiagonals on the row axis



    // ----------------- print T ---------------------

    // for (int i = 0; i < n; ++i){
    //     for (int j = 0; j < m; ++j){
    //         std::cout << T[i][j] << ' ';
    //     }
    //     std::cout << std::endl;
    // }

    //  ---------------- print H --------------------

    // for (int i = 0; i < n; ++i){
    //     for (int j = 0; j < m; ++j){
    //         std::cout << ' ' << ' ' << H[i][j] << ' ' << ' ';
    //     }
    //     std::cout << std::endl;
    // }          

    // ----------------- Traceback --------------------
    
    std::vector<std::pair<int, int>> res;

    int row = n-1;
    int col = m-1;

    while (true){ // follow indication matrix for traceback
        int gap_type = T[row][col];

        if (gap_type==gap_2){
            res.push_back(std::pair(row,0));
            row -= 1;
        }
        else if (gap_type==gap_1){
            res.push_back(std::pair(0,col));
            col -= 1;
        }
        else if (gap_type==no_gap){
            res.push_back(std::pair(row,col));
            row -= 1;
            col -= 1;
        }
        if ((row==0)&&(col==0)){
            break;
        }
    }

    std::reverse(res.begin(),res.end());

    return res;
}


#endif
