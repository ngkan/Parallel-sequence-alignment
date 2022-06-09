#ifndef NAIVE_CPP
#define NAIVE_CPP

#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace Naive {

    

int RecurrenceRelation(i,j,char a[i],char b[i]){

    int p_ij; 
    int gap1 = H[i,j-1] - 2
    int gap2 = H[i-1,j] - 2
    
    score = std::max(p_ij,std::max(H[i,j-1], H[i]));

    return score;
}


std::vector<std::pair<int, int>> ConstantGapSolver(std::string a, std::string b, std::function<int()> scoring_function, int gap_penalty) {
    
    int n = a.length(); // row number
    int m = b.length(); // col number
    int H[a,b]; // DP matrix

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
            H[i,j] = RecurrenceRelation(i,j,a[i],b[i]);
        }
    }

    // ----------------- Traceback --------------------

    


    return {};


}

std::vector<std::pair<int, int>> AffineGapSolver(std::string a, std::string b, std::function<int()> scoring_function, int constant_penalty, int gap_penalty) {
    return {};
}
};  
// namespace Naive

#endif
