#ifndef PUZZLE_SOLVER_HPP
#define PUZZLE_SOLVER_HPP

#include <unordered_map>

#include "puzzle/Board.hpp"

class Solver {
    class Solution {
    public:
        Solution() = default;

        explicit Solution(const std::vector<Board> &moves) : _moves(moves) {}

        [[nodiscard]] std::size_t moves() const { return _moves.empty() ? 0 : _moves.size() - 1; }

        using const_iterator = std::vector<Board>::const_iterator;

        [[nodiscard]] const_iterator begin() const { return _moves.begin(); }

        [[nodiscard]] const_iterator end() const { return _moves.end(); }

    private:
        std::vector<Board> _moves;
    };

public:
    static Solution solve(const Board &board);

private:
    static inline unsigned estimate(const Board &, std::unordered_map<Board, unsigned> &);
};

#endif  // PUZZLE_SOLVER_HPP
