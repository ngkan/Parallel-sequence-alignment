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


namespace Naive {

int RecurrenceRelation(std::vector<std::vector<int>>& T,std::vector<std::vector<int>>& H, int i,int j,char a_i, char b_j){

    int p_ij, score; 
    // match check
    if (a_i==b_j){
        p_ij = 1;
    }
    else{
        p_ij = -1;
    }
    // score computation
    int no_gap_sc = H[i-1][j-1] + p_ij;
    int gap_1_sc = H[i][j-1] - 2;
    int gap_2_sc = H[i-1][j] - 2;

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
        if (gap_1_sc>gap_2_sc){
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


std::vector<std::pair<int, int>> ConstantGapSolver(std::string a, std::string b, std::function<int()> scoring_function, int gap_penalty) {
    
    int n = a.length()+1; // row number
    int m = b.length()+1; // col number

    std::vector<std::vector<int>> H(n, std::vector<int> (m)), T(n, std::vector<int> (m,0));

    // H DP matrix of score
    // T traceback matrix

    int max = std::max(n,m);
    int min = std::min(n,m);

    // ---------------- Initialization --------------

    for (int i = 0; i<min; i++){
        H[i][0] = i*g;
        H[0][i] = i*g;
    }

    if (max==n){  // extend row axis
        for (int i = min; i<max; i++ ){
            H[i][0] = i*g;
        }
    }
    else{ // extend column axis
        for (int i = min; i<max; i++ ){
            H[0][i] = i*g;
        }
    }

    // ------------------ Algorithm  ------------------

    for (int i = 1; i<n; i++){
        for (int j = 1; j<m; j++){
            H[i][j] = RecurrenceRelation(T,H,i,j,a[i],b[j]);
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
            std::cout << H[i][j] << ' ';
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

std::vector<std::pair<int, int>> AffineGapSolver(std::string a, std::string b, std::function<int()> scoring_function, int constant_penalty, int gap_penalty) {
    return {};
}
};  
// namespace Naive

#endif