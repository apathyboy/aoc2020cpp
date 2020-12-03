
#include <aoc2020/aoc2020.hpp>

#include <fmt/core.h>
#include <range/v3/all.hpp> // get everything

#include <fstream>
#include <iostream>

using namespace ranges;

using namespace aoc2020;

int main(int argc, const char** argv)
{
    if (argc != 2) {
        fmt::print("USAGE: day02 <input_file_path>\n");
    }

    {
        std::ifstream ifs{argv[1]};

        auto parse     = [](const auto& s) { return parse_password_rule_string(s); };
        auto selection = [](const auto& entry) { return is_valid_day2_part1_pw(entry); };

        auto valid = ranges::getlines(ifs) | views::transform(parse)
                     | views::filter(selection);

        fmt::print("Part 1 Solution: {}\n", distance(valid));
    }

    {
        std::ifstream ifs{argv[1]};

        auto parse     = [](const auto& s) { return parse_password_rule_string(s); };
        auto selection = [](const auto& entry) { return is_valid_day2_part2_pw(entry); };

        auto valid = ranges::getlines(ifs) | views::transform(parse)
                     | views::filter(selection);

        fmt::print("Part 2 Solution: {}\n", distance(valid));
    }

    return 0;
}
