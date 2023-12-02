#include <iostream>

#include "puzzle/Solver.hpp"

int main() {
    const auto board    = Board::create_random(4);
    std::cout << board << std::endl << board.linear_conflict();

    /*const auto solution = Solver::solve(board);
    std::cout << solution.moves() << std::endl;
    for (const auto &move : solution) {
        std::cout << move << std::endl;
    }*/
}
