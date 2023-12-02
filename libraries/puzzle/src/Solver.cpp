#include "puzzle/Solver.hpp"

#include <queue>
#include <set>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

std::vector<Board> reconstruct_answer(const Board &board, std::unordered_map<Board, Board> &parent) {
    std::vector<Board> result{board};
    while (parent[result.back()] != result.back()) {
        result.push_back(parent[result.back()]);
    }
    std::reverse(result.begin(), result.end());
    return result;
}

unsigned inline Solver::estimate(const Board &y, std::unordered_map<Board, unsigned> &shortest_path) {
    return shortest_path[y] + (5 * y._manh >> 1);
}

Solver::Solution Solver::solve(const Board &board) {
    if (!board.is_solvable()) {
        return {};
    }

    std::unordered_map<Board, Board> parent;
    std::unordered_set<Board> used;
    std::unordered_map<Board, unsigned> shortest_path;

    // many usages
    auto compare_pairs = [&shortest_path](const Board &x, const Board &y) {
        auto y_estimate = estimate(y, shortest_path);
        auto x_estimate = estimate(x, shortest_path);
        return (x_estimate < y_estimate) || (x_estimate == y_estimate && x._hash < y._hash);
    };

    // priority queue is implemented by std::set because we need to modify specific member
    // of queue (update shortest_path) without breaking the invariant - so we erase it and insert back
    std::set<Board, decltype(compare_pairs)> queue(compare_pairs);

    queue.insert(board);
    parent[board]        = board;
    shortest_path[board] = 0;

    while (!queue.empty()) {
        const Board current = *queue.begin();
        queue.erase(queue.begin());
        used.insert(current);

        if (current._is_goal) {
            return Solution(reconstruct_answer(current, parent));
        }

        unsigned score = shortest_path[current] + 1;

        // maybe we can cache next_moves somehow
        for (const auto &v : current.next_moves()) {
            if (!used.contains(v) || score < shortest_path[v]) {
                // not necessary to check if queue contains v
                queue.erase(v);
                parent[v]        = current;
                shortest_path[v] = score;
                queue.insert(v);
            }
        }
    }

    throw std::invalid_argument("given configuration doesn't have solutions");
}
