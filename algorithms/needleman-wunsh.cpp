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

// ----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

class SimplePool {
        unsigned int num_workers;
        std::vector<std::thread> workers;
        SafeUnboundedQueue<std::function<bool()> > tasks;
        // the suggestion is to use the returned bool value to distinguish
        // between usuall tasks and stopper tasks
        //
        // std::optional<std::function<void()> >
        // struct task {std::function<void()>, bool}
        // ...
        // stopper
        // push([]() -> bool {return false;})
        // usual task
        // push([]() -> bool {do some work with f and args; return true;})


        void do_work();
        // create workers: workers[i] = std::thread(&do_work)

    public:
        SimplePool(unsigned int num_workers = 0);
        ~SimplePool();
        template <class F, class... Args>
        void push(F f, Args ... args);
        void stop();
};

void SimplePool::do_work() {

    while(true){
        std::function<bool()> t = tasks.pop();
        bool  res = t(); // execute task and retrieve bool flag
        if (res==false){ // stopper task
            return; // stop taking new tasks
        }
    }

}

SimplePool::SimplePool(unsigned int num_workers) {
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

template <class F, class... Args>
void SimplePool::push(F f, Args ... args) {

    tasks.push([args..., f]() -> bool {f(args...); return true;});

}

void SimplePool::stop() {

    // push n stopper tasks
    for (int i = 0; i<num_workers; i++){
        tasks.push([]() -> bool {return false;}); 
    }

    // join all threads
    for (int i = 0; i<num_workers; i++){
        workers[i].join();
    }
}

//-----------------------------------------------------------------------------

std::vector<std::pair<int, int>> DW_NW(std::string a, std::string b, std::function<int(char, char)> scoring_function, int gap_penalty) {
    
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

    // ----------------- print T ---------------------

    for (int i = 0; i < n; ++i){
        for (int j = 0; j < m; ++j){
            std::cout << T[i][j] << ' ';
        }
        std::cout << std::endl;
    }

    // ---------------- print H --------------------

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
