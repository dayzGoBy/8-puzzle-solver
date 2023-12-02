#ifndef PUZZLE_BOARD_HPP
#define PUZZLE_BOARD_HPP

#include <string>
#include <vector>

class Board {
public:
    using Table = std::vector<std::vector<unsigned>>;

    static Board create_goal(unsigned size);

    static Board create_random(unsigned size);

    Board();

    explicit Board(const Table &data)
        : _table(data)
        , _size(data.size())
        , _manh(calculate_manhattan(data))
        , _hash(calculate_hash(data))
        , _zero(find_zero(data))
        , _is_goal(is_goal()) {}
        //, _linear_conflict(calculate_linear_conflict(data)) {}

    Board(const Board &that)
        : _table(that._table)
        , _size(that._table.size())
        , _manh(calculate_manhattan(that._table))
        , _hash(calculate_hash(that._table))
        , _zero(find_zero(that._table))
        , _is_goal(is_goal()) {}
        //, _linear_conflict(calculate_linear_conflict(that._table)) {}

    Board &operator=(Board const &) = default;

    [[nodiscard]] std::size_t size() const;

    [[nodiscard]] bool is_goal() const;

    [[nodiscard]] unsigned hamming() const;

    [[nodiscard]] unsigned manhattan() const { return _manh; }

    [[nodiscard]] unsigned linear_conflict() const { return _linear_conflict; }

    [[nodiscard]] std::string to_string() const;

    [[nodiscard]] bool is_solvable() const;

    [[nodiscard]] std::vector<Board> next_moves() const;

    const std::vector<unsigned> &operator[](std::size_t index) const;

    friend bool operator==(const Board &lhs, const Board &rhs);

    friend bool operator!=(const Board &lhs, const Board &rhs);

    friend std::ostream &operator<<(std::ostream &out, const Board &board) { return out << board.to_string(); }

    friend struct std::hash<Board>;

    friend class Solver;

    static std::vector<unsigned> reshape(const std::vector<std::vector<unsigned>> &array);

private:
    Table _table;
    std::size_t _size;
    unsigned _manh = 0;
    unsigned _hash = 0;
    std::pair<unsigned, unsigned> _zero;
    bool _is_goal                = true;
    std::size_t _linear_conflict = 0;

    [[nodiscard]] static unsigned calculate_manhattan(const Table &data);

    [[nodiscard]] static std::size_t calculate_hash(const Table &data);

    [[nodiscard]] static std::size_t calculate_linear_conflict(const Table &data);

    static std::pair<unsigned, unsigned> find_zero(const Table &data);
};

namespace std {
template <>
struct hash<Board> {
    std::size_t operator()(const Board &board) const { return board._hash; }
};
}  // namespace std

#endif  // PUZZLE_BOARD_HPP
