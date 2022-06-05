#ifndef NAIVE_CPP
#define NAIVE_CPP

#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace Naive {
std::vector<std::pair<int, int>> ConstantGapSolver(std::string a, std::string b, std::function<int()> scoring_function, int gap_penalty) {
    return {};
}

std::vector<std::pair<int, int>> AffineGapSolver(std::string a, std::string b, std::function<int()> scoring_function, int constant_penalty, int gap_penalty) {
    return {};
}
};  // namespace Naive

#endif
