#ifndef NAIVE_CPP
#define NAIVE_CPP

#include <functional>
#include <string>
#include <utility>
#include <vector>

#define gap_1 1 // (i,j-1)
#define gap_2 2 // (i-1,j)
#define no_gap 3
#define g -2


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
