#include <fmt/format.h>
#include <range/v3/all.hpp>

#include <fstream>

#include <iostream>
#include <map>
#include <set>

namespace rs = ranges;
namespace rv = ranges::views;

struct tile {
    int64_t                        id;
    std::vector<std::vector<char>> image_data;
};

int parse_tile_id(const std::string& s)
{
    return std::stoi(s.substr(5, s.length() - 1));
}

std::vector<tile> read_input(std::istream&& input)
{
    return rs::getlines(input) | rv::split("") | rv::transform([](auto&& rng) {
               auto v = rng | rv::transform([](auto&& s) { return s | rs::to<std::string>; })
                        | rs::to_vector;

               tile t{parse_tile_id(v[0]), rv::tail(v) | rs::to<std::vector<std::vector<char>>>};

               return t;
           })
           | rs::to_vector;
}


template <typename T>
auto rotate_tile(const std::vector<std::vector<T>>& grid)
{
    return rv::iota(0, static_cast<int>(grid[0].size())) | rv::transform([&grid](int i) {
               return grid | rv::transform([i](auto&& t) { return t[i]; }) | rv::reverse | rs::to_vector;
           })
           | rs::to_vector;
}

template <typename T>
auto flip(const std::vector<std::vector<T>>& grid)
{
    return grid | rv::transform([](auto&& s) { return s | rv::reverse | rs::to_vector; })
           | rs::to_vector;
}

std::vector<char> left_side(const tile& t)
{
    return t.image_data | rv::transform([](auto&& t) { return t[0]; }) | rs::to_vector;
}

std::vector<char> right_side(const tile& t)
{
    int pos = static_cast<int>(t.image_data[0].size() - 1);
    return t.image_data | rv::transform([pos](auto&& t) { return t[pos]; }) | rs::to_vector;
}

bool is_top_neighbor(const tile& t1, const tile& t2)
{
    return rs::front(t1.image_data) == rs::back(t2.image_data);
}

bool is_bottom_neighbor(const tile& t1, const tile& t2)
{
    return rs::back(t1.image_data) == rs::front(t2.image_data);
}

bool is_left_neighbor(const tile& t1, const tile& t2)
{
    return left_side(t1) == right_side(t2);
}

bool is_right_neighbor(const tile& t1, const tile& t2)
{
    return right_side(t1) == left_side(t2);
}

bool is_neighbor(const tile& t1, tile& t2)
{
    for (int i : rv::iota(0, 8)) {
        // test at all sides and rotations/flip
        if (is_top_neighbor(t1, t2) || is_bottom_neighbor(t1, t2) || is_left_neighbor(t1, t2)
            || is_right_neighbor(t1, t2)) {
            return true;
        }

        t2.image_data = i % 4 == 0 ? flip(t2.image_data) : rotate_tile(t2.image_data);
    }

    return false;
}

std::vector<tile> find_neighbors(std::vector<tile> tiles, const tile& t)
{
    std::vector<tile> neighbors;
    for (auto check_tile : tiles) {
        if (t.id == check_tile.id) continue;
        if (is_neighbor(t, check_tile)) { neighbors.push_back(check_tile); }
    }
    return neighbors;
}

auto build_neighbor_map(const std::vector<tile>& tiles)
{
    return tiles
           | rv::transform([&tiles](auto&& t) { return std::make_pair(t.id, find_neighbors(tiles, t)); })
           | rs::to<std::map<int64_t, std::vector<tile>>>;
}

int64_t part1(std::vector<tile> tiles)
{
    auto neighbor_map = build_neighbor_map(tiles);

    return rs::accumulate(
        neighbor_map | rv::filter([](const auto& p) { return p.second.size() == 2; })
            | rv::transform([](auto&& p) { return p.first; }),
        int64_t{1},
        std::multiplies<>{});
}

int64_t part2(std::vector<tile> tiles)
{
    (tiles);
    return 0;
}

#ifndef UNIT_TESTING

int main()
{
    fmt::print("Advent of Code 2020 - Day 20\n");

    std::string input_path = "days/day20/example.in";

    auto input = read_input(std::ifstream{input_path});

    fmt::print("Part 1 Solution: {}\n", part1(input));
    fmt::print("Part 2 Solution: {}\n", part2(input));

    return 0;
}

#else

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <sstream>

TEST_CASE("Can rotate tile")
{
    tile t;
    t.id         = 0;
    t.image_data = std::vector{
        std::vector{'#', '#', '#', '#', '#', '.', '.', '.', '.', '.'},
        std::vector{'.', '.', '.', '.', '.', '#', '#', '#', '#', '#'},
        std::vector{'#', '#', '#', '#', '#', '.', '.', '.', '.', '.'},
        std::vector{'.', '.', '.', '.', '.', '#', '#', '#', '#', '#'},
        std::vector{'#', '#', '#', '#', '#', '.', '.', '.', '.', '.'},
        std::vector{'.', '.', '.', '.', '.', '#', '#', '#', '#', '#'},
        std::vector{'#', '#', '#', '#', '#', '.', '.', '.', '.', '.'},
        std::vector{'.', '.', '.', '.', '.', '#', '#', '#', '#', '#'},
        std::vector{'#', '#', '#', '#', '#', '.', '.', '.', '.', '.'},
        std::vector{'.', '.', '.', '.', '.', '#', '#', '#', '#', '#'}};

    auto expected = std::vector{
        std::vector{'.', '#', '.', '#', '.', '#', '.', '#', '.', '#'},
        std::vector{'.', '#', '.', '#', '.', '#', '.', '#', '.', '#'},
        std::vector{'.', '#', '.', '#', '.', '#', '.', '#', '.', '#'},
        std::vector{'.', '#', '.', '#', '.', '#', '.', '#', '.', '#'},
        std::vector{'.', '#', '.', '#', '.', '#', '.', '#', '.', '#'},
        std::vector{'#', '.', '#', '.', '#', '.', '#', '.', '#', '.'},
        std::vector{'#', '.', '#', '.', '#', '.', '#', '.', '#', '.'},
        std::vector{'#', '.', '#', '.', '#', '.', '#', '.', '#', '.'},
        std::vector{'#', '.', '#', '.', '#', '.', '#', '.', '#', '.'},
        std::vector{'#', '.', '#', '.', '#', '.', '#', '.', '#', '.'}};

    REQUIRE(expected == rotate_tile(t.image_data));
}

TEST_CASE("Can flip tile")
{
    tile t;
    t.id         = 0;
    t.image_data = std::vector{
        std::vector{'#', '#', '#', '#', '#', '.', '.', '.', '.', '.'},
        std::vector{'.', '.', '.', '.', '.', '#', '#', '#', '#', '#'},
        std::vector{'#', '#', '#', '#', '#', '.', '.', '.', '.', '.'},
        std::vector{'.', '.', '.', '.', '.', '#', '#', '#', '#', '#'},
        std::vector{'#', '#', '#', '#', '#', '.', '.', '.', '.', '.'},
        std::vector{'.', '.', '.', '.', '.', '#', '#', '#', '#', '#'},
        std::vector{'#', '#', '#', '#', '#', '.', '.', '.', '.', '.'},
        std::vector{'.', '.', '.', '.', '.', '#', '#', '#', '#', '#'},
        std::vector{'#', '#', '#', '#', '#', '.', '.', '.', '.', '.'},
        std::vector{'.', '.', '.', '.', '.', '#', '#', '#', '#', '#'}};

    auto expected = std::vector{
        std::vector{'.', '.', '.', '.', '.', '#', '#', '#', '#', '#'},
        std::vector{'#', '#', '#', '#', '#', '.', '.', '.', '.', '.'},
        std::vector{'.', '.', '.', '.', '.', '#', '#', '#', '#', '#'},
        std::vector{'#', '#', '#', '#', '#', '.', '.', '.', '.', '.'},
        std::vector{'.', '.', '.', '.', '.', '#', '#', '#', '#', '#'},
        std::vector{'#', '#', '#', '#', '#', '.', '.', '.', '.', '.'},
        std::vector{'.', '.', '.', '.', '.', '#', '#', '#', '#', '#'},
        std::vector{'#', '#', '#', '#', '#', '.', '.', '.', '.', '.'},
        std::vector{'.', '.', '.', '.', '.', '#', '#', '#', '#', '#'},
        std::vector{'#', '#', '#', '#', '#', '.', '.', '.', '.', '.'}};

    REQUIRE(expected == flip(t.image_data));
}

TEST_CASE("Can get left side of tile")
{
    tile t;
    t.id         = 0;
    t.image_data = std::vector{
        std::vector{'.', '.', '#', '#', '.', '#', '.', '.', '#', '.'},
        std::vector{'#', '#', '.', '.', '#', '.', '.', '.', '.', '.'},
        std::vector{'#', '.', '.', '.', '#', '#', '.', '.', '#', '.'},
        std::vector{'#', '#', '#', '#', '.', '#', '.', '.', '.', '#'},
        std::vector{'#', '#', '.', '#', '#', '.', '#', '#', '#', '.'},
        std::vector{'#', '#', '.', '.', '.', '#', '.', '#', '#', '#'},
        std::vector{'.', '#', '.', '#', '.', '#', '.', '.', '#', '#'},
        std::vector{'.', '.', '#', '.', '.', '.', '.', '#', '.', '.'},
        std::vector{'#', '#', '#', '.', '.', '.', '#', '.', '#', '.'},
        std::vector{'.', '.', '#', '#', '#', '.', '.', '#', '#', '#'}};

    std::vector expected{'.', '#', '#', '#', '#', '#', '.', '.', '#', '.'};

    REQUIRE(expected == left_side(t));
}

TEST_CASE("Can get right side of tile")
{
    tile t;
    t.id         = 0;
    t.image_data = std::vector{
        std::vector{'.', '.', '#', '#', '.', '#', '.', '.', '#', '.'},
        std::vector{'#', '#', '.', '.', '#', '.', '.', '.', '.', '.'},
        std::vector{'#', '.', '.', '.', '#', '#', '.', '.', '#', '.'},
        std::vector{'#', '#', '#', '#', '.', '#', '.', '.', '.', '#'},
        std::vector{'#', '#', '.', '#', '#', '.', '#', '#', '#', '.'},
        std::vector{'#', '#', '.', '.', '.', '#', '.', '#', '#', '#'},
        std::vector{'.', '#', '.', '#', '.', '#', '.', '.', '#', '#'},
        std::vector{'.', '.', '#', '.', '.', '.', '.', '#', '.', '.'},
        std::vector{'#', '#', '#', '.', '.', '.', '#', '.', '#', '.'},
        std::vector{'.', '.', '#', '#', '#', '.', '.', '#', '#', '#'}};

    std::vector expected{'.', '.', '.', '#', '.', '#', '#', '.', '.', '#'};

    REQUIRE(expected == right_side(t));
}

TEST_CASE("Can solve part 1 example")
{
    std::stringstream ss;

    ss << R"(Tile 2311:
..##.#..#.
##..#.....
#...##..#.
####.#...#
##.##.###.
##...#.###
.#.#.#..##
..#....#..
###...#.#.
..###..###

Tile 1951:
#.##...##.
#.####...#
.....#..##
#...######
.##.#....#
.###.#####
###.##.##.
.###....#.
..#.#..#.#
#...##.#..

Tile 1171:
####...##.
#..##.#..#
##.#..#.#.
.###.####.
..###.####
.##....##.
.#...####.
#.##.####.
####..#...
.....##...

Tile 1427:
###.##.#..
.#..#.##..
.#.##.#..#
#.#.#.##.#
....#...##
...##..##.
...#.#####
.#.####.#.
..#..###.#
..##.#..#.

Tile 1489:
##.#.#....
..##...#..
.##..##...
..#...#...
#####...#.
#..#.#.#.#
...#.#.#..
##.#...##.
..##.##.##
###.##.#..

Tile 2473:
#....####.
#..#.##...
#.##..#...
######.#.#
.#...#.#.#
.#########
.###.#..#.
########.#
##...##.#.
..###.#.#.

Tile 2971:
..#.#....#
#...###...
#.#.###...
##.##..#..
.#####..##
.#..####.#
#..#.#..#.
..####.###
..#.#.###.
...#.#.#.#

Tile 2729:
...#.#.#.#
####.#....
..#.#.....
....#..#.#
.##..##.#.
.#.####...
####.#.#..
##.####...
##..#.##..
#.##...##.

Tile 3079:
#.#.#####.
.#..######
..#.......
######....
####.#..#.
.#...#.##.
#.#####.##
..#.###...
..#.......
..#.###...)";

    auto input = read_input(std::move(ss));

    REQUIRE(20899048083289 == part1(input));
}

TEST_CASE("Can solve part 2 example")
{
    std::stringstream ss;

    ss << R"(Tile 2311:
..##.#..#.
##..#.....
#...##..#.
####.#...#
##.##.###.
##...#.###
.#.#.#..##
..#....#..
###...#.#.
..###..###

Tile 1951:
#.##...##.
#.####...#
.....#..##
#...######
.##.#....#
.###.#####
###.##.##.
.###....#.
..#.#..#.#
#...##.#..

Tile 1171:
####...##.
#..##.#..#
##.#..#.#.
.###.####.
..###.####
.##....##.
.#...####.
#.##.####.
####..#...
.....##...

Tile 1427:
###.##.#..
.#..#.##..
.#.##.#..#
#.#.#.##.#
....#...##
...##..##.
...#.#####
.#.####.#.
..#..###.#
..##.#..#.

Tile 1489:
##.#.#....
..##...#..
.##..##...
..#...#...
#####...#.
#..#.#.#.#
...#.#.#..
##.#...##.
..##.##.##
###.##.#..

Tile 2473:
#....####.
#..#.##...
#.##..#...
######.#.#
.#...#.#.#
.#########
.###.#..#.
########.#
##...##.#.
..###.#.#.

Tile 2971:
..#.#....#
#...###...
#.#.###...
##.##..#..
.#####..##
.#..####.#
#..#.#..#.
..####.###
..#.#.###.
...#.#.#.#

Tile 2729:
...#.#.#.#
####.#....
..#.#.....
....#..#.#
.##..##.#.
.#.####...
####.#.#..
##.####...
##..#.##..
#.##...##.

Tile 3079:
#.#.#####.
.#..######
..#.......
######....
####.#..#.
.#...#.##.
#.#####.##
..#.###...
..#.......
..#.###...)";

    auto input = read_input(std::move(ss));

    REQUIRE(0 == part2(input));
}

#endif
