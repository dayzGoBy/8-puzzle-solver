#include "puzzle/Board.hpp"

#include <algorithm>
#include <numeric>
#include <random>
#include <stdexcept>

Board Board::create_goal(const unsigned size) {
    std::vector<std::vector<unsigned>> data(size, std::vector<unsigned>(size));
    std::size_t number = 1;
    for (auto &vec : data) {
        std::iota(vec.begin(), vec.end(), number);
        number += size;
    }
    data[size - 1][size - 1] = 0;
    return Board(data);
}

Board Board::create_random(const unsigned size) {
    std::vector<std::vector<unsigned>> data(size, std::vector<unsigned>(size));
    std::vector<unsigned> permutation(size * size);
    std::iota(permutation.begin(), permutation.end(), 0);
    std::shuffle(permutation.begin(), permutation.end(), std::mt19937(std::random_device()()));
    auto index = permutation.begin();
    for (auto &vec : data) {
        std::copy(index, index + size, vec.begin());
        index += size;
    }
    return Board(data);
}

Board::Board() {
    this->_table = Table();
    this->_size  = 0;
}

std::size_t Board::size() const {
    return _size;
}

bool Board::is_goal() const {
    unsigned number = 1;
    for (auto &vec : _table) {
        for (unsigned value : vec) {
            if (value != (number++ % (_size * _size))) {
                return false;
            }
        }
    }
    return true;
}

unsigned Board::hamming() const {
    unsigned res = 0;
    if (!_size) {
        return 0;
    }
    for (std::size_t i = 0; i < _size * _size; i++) {
        if ((_table[i / _size][i % _size] && (i + 1 != _table[i / _size][i % _size])) ||
            (!_table[i / _size][i % _size] && (i != _size * _size - 1))) {
            res++;
        }
    }
    return res;
}

unsigned inversions_number(std::vector<unsigned> &array, unsigned begin, unsigned end) {
    if (end - begin <= 1) {
        return 0;
    }
    unsigned mid = (end + begin) / 2;
    unsigned res = inversions_number(array, begin, mid);
    res += inversions_number(array, mid, end);

    std::size_t i = begin;
    std::size_t j = mid;
    std::size_t k = 0;

    std::vector<unsigned> result(end - begin);

    while (i < mid && j < end) {
        if (array[j] < array[i]) {
            result[k++] = array[j++];
            res += (mid - i);
        } else {
            result[k++] = array[i++];
        }
    }

    while (i < mid) {
        result[k++] = array[i++];
    }

    while (j < end) {
        result[k++] = array[j++];
    }

    std::copy(result.begin(), result.end(), array.begin() + begin);

    return res;
}

std::vector<unsigned> Board::reshape(const Board::Table &array) {
    if (array.empty()) {
        return {};
    }
    unsigned n = array[0].size();
    std::vector<unsigned> result(n * array.size());
    auto index = result.begin();
    for (auto &vec : array) {
        std::copy(vec.begin(), vec.begin() + n, index);
        index += n;
    }
    return result;
}

unsigned Board::calculate_manhattan(const Table &data) {
    unsigned res      = 0;
    std::size_t _size = data.size();
    if (data.empty()) {
        return 0;
    }
    for (std::size_t i = 0; i < _size * _size; i++) {
        long num = static_cast<long>(data[i / _size][i % _size]);
        if (num) {
            res += std::abs(static_cast<long>((num - 1) % _size - i % _size)) +
                   std::abs(static_cast<long>((num - 1) / _size - i / _size));
        }
    }
    return res;
}

std::string Board::to_string() const {
    std::string result = "\n";
    for (auto &vec : _table) {
        for (unsigned e : vec) {
            result += std::to_string(e) + ' ';
        }
        result += '\n';
    }
    return result;
}

bool Board::is_solvable() const {
    if (!_size || _size == 1) {
        return true;
    }
    std::vector<unsigned> reshaped(reshape(_table));
    unsigned zero = std::find(reshaped.begin(), reshaped.end(), 0) - reshaped.begin();
    std::vector<unsigned> non_zero(_size * _size - 1);
    std::copy(reshaped.begin(), reshaped.begin() + zero, non_zero.begin());
    std::copy(reshaped.begin() + zero + 1, reshaped.end(), non_zero.begin() + zero);
    return (((_size - 1) % 2) * (zero / _size + 1) + inversions_number(non_zero, 0, _size * _size - 1)) % 2 == 0;
}

bool operator==(const Board &lhs, const Board &rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    for (std::size_t i = 0; i < lhs.size(); i++) {
        if (!std::equal(lhs._table[i].begin(), lhs._table[i].end(), rhs._table[i].begin())) {
            return false;
        }
    }
    return true;
}

bool operator!=(const Board &lhs, const Board &rhs) {
    return !(lhs == rhs);
}

const std::vector<unsigned> &Board::operator[](std::size_t index) const {
    return _table[index];
}

inline void generate_swapped(const Board::Table &from, std::vector<Board> &where, std::size_t x1, std::size_t y1,
                             std::size_t x2, std::size_t y2) {
    Board::Table new_table(from);
    std::swap(new_table[x1][y1], new_table[x2][y2]);
    where.emplace_back(new_table);
}

std::pair<unsigned, unsigned> Board::find_zero(const Table &data) {
    for (std::size_t i = 0; i < data.size(); i++) {
        unsigned find = std::find(data[i].begin(), data[i].end(), 0) - data[i].begin();
        if (find < data[i].size()) {
            return {i, find};
        }
    }
    return {-1, -1};
}

std::vector<Board> Board::next_moves() const {
    unsigned x = _zero.first;
    unsigned y = _zero.second;

    std::vector<Board> result;
    if (x > 0) {
        generate_swapped(_table, result, x - 1, y, x, y);
    }
    if (x < _size - 1) {
        generate_swapped(_table, result, x + 1, y, x, y);
    }
    if (y > 0) {
        generate_swapped(_table, result, x, y - 1, x, y);
    }
    if (y < _size - 1) {
        generate_swapped(_table, result, x, y + 1, x, y);
    }
    return result;
}

std::size_t Board::calculate_hash(const Table &data) {
    std::vector<unsigned> reshaped = reshape(data);
    std::size_t seed               = reshaped.size();
    for (auto &i : reshaped) {
        seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
}

Board::Table transpose(const Board::Table &array) {
    Board::Table result(array);
    for (std::size_t x = 0; x < array.size(); x++) {
        for (std::size_t y = 0; y < array[x].size(); y++) {
            std::swap(result[x][y], result[y][x]);
        }
    }
    return result;
}

std::size_t horizontal_conflict(const Board::Table &data) {
    std::size_t s = data.size();
    std::size_t res = 0;
    for (std::size_t x = 0; x < s; x++) {
        for (std::size_t one = 0; one < s; one++) {
            auto val_one = data[x][one] - 1;
            if (val_one >= (x + 1) * s || val_one < x * s) {
                continue;
            }
            for (std::size_t two = one + 1; two < data[0].size(); two++) {
                auto val_two = data[x][two] - 1;
                if (val_two >= (x + 1) * s || val_two < x * s) {
                    continue;
                }
                if (val_one > val_two) {
                    res++;
                }
            }
        }
    }

    return res;
}

std::size_t Board::calculate_linear_conflict(const Board::Table &data) {
    return 2 * (horizontal_conflict(data) + horizontal_conflict(transpose(data)));
}


