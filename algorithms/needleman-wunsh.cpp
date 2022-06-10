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

// --------------------- SafeUnboundedQueue for Diagonal Wavefront --------------------------------

template <class E> 
class SafeUnboundedQueue {
        std::queue<E> elements;
        std::mutex lock;
        std::condition_variable not_empty;
    public: 
        SafeUnboundedQueue<E>(){}
        void push(const E& element);
        E pop ();
        bool is_empty() const {return this->elements.empty();}
};

template <class E>
void SafeUnboundedQueue<E>::push(const E& element) {

    std::unique_lock<std::mutex> lk(lock);

    bool was_empty = is_empty();
    elements.push(element);
    // signal that queue is not longer empty
    if (was_empty){ 
        not_empty.notify_all();
    }

}

template <class E> 
E SafeUnboundedQueue<E>::pop() {
    
    std::unique_lock<std::mutex> lk(lock);

    while(is_empty()){
        not_empty.wait(lk);
    }
    // hold the lock, queue not empty
    E last = elements.front();
    elements.pop();
    return last;
}

//---------------------- Thread pool for Diagonal Wavefront --------------------------------

class SimplePool {
        unsigned int num_workers;
        std::vector<std::thread> workers;
        void do_work();

    public:
        SafeUnboundedQueue< std::pair<int,int> > tasks;
        SimplePool(unsigned int num_workers, int n, int m );
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

SimplePool::SimplePool(unsigned int num_workers, int n, int m) {

    std::vector<std::vector<int>> h(n, std::vector<int> (m));
    std::vector<std::vector<int>> t(n, std::vector<int> (m));
    
    this->H = h;
    this->T = t;

    this->num_workers = num_workers;

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

//----------------------Diagonal Wavefront applied to Needleman-Wunsh ------------------------------

std::vector<std::pair<int, int>> DW_NW(std::string a, std::string b, std::function<int(char, char)> scoring_function, int gap_penalty) {
    
    int n = a.length()+1; // row number
    int m = b.length()+1; // col number

    int max = std::max(n,m);
    int min = std::min(n,m);

    // ---------------- Initialization --------------

    unsigned int num_workers = min-1;
    SimplePool worker_pool= SimplePool(num_workers, n , m);

    worker_pool.scoring_function = scoring_function;
    worker_pool.a = a;
    worker_pool.b = b;
    worker_pool.gap_penalty = gap_penalty;

    for (int i = 0; i<min; i++){
        worker_pool.H[i][0] = i*gap_penalty;
        worker_pool.H[0][i] = i*gap_penalty;
        worker_pool.T[i][0] = 0;
        worker_pool.T[0][i] = 0;
    }

    if (max==n){  // extend row axis
        for (int i = min; i<max; i++ ){
            worker_pool.H[i][0] = i*gap_penalty;
            worker_pool.T[i][0] = 0;
        }
    }
    else{ // extend column axis
        for (int i = min; i<max; i++ ){
            worker_pool.H[0][i] = i*gap_penalty;
            worker_pool.T[0][i] = 0;
        }
    }

    // ------------------ Algorithm  ------------------

    int r, c;
    // iterate over rows
    for (int i = 1; i<n; i++){
        // construct anti-diagonal and push tasks
        r = i;
        c = 1;
        std::cout << "diag " << i << std::endl;
        while ((1<=r)&&(c<m)){
            std::cout << "(" << r << ' ' << c << ")" << ' ';
            worker_pool.push(std::pair(r,c));
            r -= 1;
            c += 1;
        }

        while(!worker_pool.tasks.is_empty()){
            ;
        }
    }
    // then over columns
    for (int j = 2; j<m; j++){
        // construct anti-diagonal and push tasks
        r = n-1;
        c = j;
        while ((1<=r)&&(c<m)){
            worker_pool.push(std::pair(r,c));
            r -= 1;
            c += 1;
        }

        while(!worker_pool.tasks.is_empty()){
            ;
        }
    }

    worker_pool.stop();

    std::vector<std::vector<int>> H,T;
    H = worker_pool.H;
    T = worker_pool.T;

    std::cout<< "finished work" << std::endl;

    std::cout << "T row " << worker_pool.T.size() << " col" << worker_pool.T[0].size() << std::endl;
    // --------------- print H size -----------
    std::cout << "H row " << worker_pool.H.size() << " col" << worker_pool.H[0].size() << std::endl;


    // ----------------- print T ---------------------

    for (int i = 0; i < n; ++i){
        for (int j = 0; j < m; ++j){
            std::cout << T[i][j] << ' ';
        }
        std::cout << std::endl;
    }

    // // ---------------- print H --------------------

    for (int i = 0; i < n; ++i){
        for (int j = 0; j < m; ++j){
            std::cout << ' ' << ' ' << H[i][j] << ' ' << ' ';
        }
        std::cout << std::endl;
    }          

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
