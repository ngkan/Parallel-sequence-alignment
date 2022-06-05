#ifndef RAJKO_ALURU_CPP
#define RAJKO_ALURU_CPP

#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace RajkoAluru {
std::vector<std::pair<int, int>> ConstantGapSolver(std::string a, std::string b, std::function<int()> scoring_function, int gap_penalty) {
    return {{1, 1}, {2, 2}, {3, 3}};
}

std::vector<std::pair<int, int>> AffineGapSolver(std::string a, std::string b, std::function<int()> scoring_function, int constant_penalty, int gap_penalty) {
    return {};
}
};  // namespace RajkoAluru

#endif
