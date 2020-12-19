#include <fmt/format.h>
#include <range/v3/all.hpp>

#include <cctype>
#include <fstream>
#include <iostream>
#include <regex>
#include <unordered_map>
#include <vector>

namespace rs = ranges;
namespace rv = ranges::views;

namespace day19 {

struct rule {
    enum class TYPE { MATCH, SUBRULE };

    TYPE type;

    std::vector<std::vector<int>> subrules;
    char                          match;
};

} // namespace day19

auto read_rules(std::istream& input)
{
    std::string                          tmp;
    std::unordered_map<int, day19::rule> rules;

    while (std::getline(input, tmp)) {
        if (tmp == "") break;

        std::smatch m;

        day19::rule r;
        r.type = day19::rule::TYPE::SUBRULE;

        auto pos = tmp.find_first_of(": ");

        int rule_num = std::stoi(tmp.substr(0, pos));

        auto rest = tmp.substr(pos + 2);

        if (rest[0] == '\"') {
            r.type  = day19::rule::TYPE::MATCH;
            r.match = rest[1];
        }
        else if (std::regex_match(rest, m, std::regex(R"((\d+) (\d+) \| (\d+) (\d+))"))) {
            r.subrules.push_back(std::vector{std::stoi(m.str(1)), std::stoi(m.str(2))});
            r.subrules.push_back(std::vector{std::stoi(m.str(3)), std::stoi(m.str(4))});
        }
        else if (std::regex_match(rest, m, std::regex(R"((\d+) \| (\d+))"))) {
            r.subrules.push_back(std::vector{std::stoi(m.str(1))});
            r.subrules.push_back(std::vector{std::stoi(m.str(2))});
        }
        else {
            auto subrules = rest | rv::split(' ')
                            | rv::transform([](auto&& s) { return std::stoi(s | rs::to<std::string>); })
                            | rs::to_vector;

            r.subrules.push_back(subrules);
        }

        rules.emplace(std::make_pair(rule_num, r));
    }

    return rules;
}

auto read_input(std::istream&& input)
{
    auto rules = read_rules(input);

    auto messages = rs::getlines(input) | rv::transform([](auto&& s) { return s | rs::to<std::string>; })
                    | rs::to<std::vector<std::string>>;

    return std::make_pair(std::move(rules), std::move(messages));
}

bool match(
    const std::unordered_map<int, day19::rule>& rules,
    const day19::rule&                          r,
    const std::string&                          s,
    int64_t&                                    idx)
{
    if (r.type == day19::rule::TYPE::MATCH) { return s[idx++] == r.match; }

    return rs::any_of(r.subrules, [&rules, &s, &idx](const auto& sr) {
        int64_t tmp = idx;
        if (!rs::all_of(sr | rv::enumerate, [&rules, &s, &idx](const auto& p) {
                if (idx == static_cast<int64_t>(s.length())) {
                    // break out
                    return p.first == 2;
                }
                return match(rules, rules.at(p.second), s, idx);
            })) {
            idx = tmp;
            return false;
        }
        return true;
    });
}

bool match(const std::unordered_map<int, day19::rule>& rules, const day19::rule& r, const std::string& s)
{
    int64_t idx = 0;
    return match(rules, r, s, idx) && idx == static_cast<int64_t>(s.size());
}

int64_t
part1(const std::unordered_map<int, day19::rule>& rules, const std::vector<std::string>& messages)
{
    return rs::distance(
        messages | rv::filter([&rules](const auto& s) { return match(rules, rules.at(0), s); }));
}

int64_t part2(std::unordered_map<int, day19::rule> rules, const std::vector<std::string>& messages)
{
    day19::rule r8;
    r8.type     = day19::rule::TYPE::SUBRULE;
    r8.subrules = std::vector{std::vector{42}, std::vector{42, 8}};

    day19::rule r11;
    r11.type     = day19::rule::TYPE::SUBRULE;
    r11.subrules = std::vector{std::vector{42, 31}, std::vector{42, 11, 31}};

    rules[8]  = r8;
    rules[11] = r11;

    return rs::distance(
        messages | rv::filter([&rules](const auto& s) { return match(rules, rules.at(0), s); }));
}

#ifndef UNIT_TESTING

int main()
{
    fmt::print("Advent of Code 2020 - Day 19\n");

    std::string input_path = "days/day19/puzzle.in";

    auto [rules, messages] = read_input(std::ifstream{input_path});

    fmt::print("Part 1 Solution: {}\n", part1(rules, messages));
    fmt::print("Part 2 Solution: {}\n", part2(rules, messages));

    return 0;
}

#else

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <sstream>

TEST_CASE("Can match part2 string")
{
    std::stringstream ss;

    ss << R"(42: 9 14 | 10 1
9: 14 27 | 1 26
10: 23 14 | 28 1
1: "a"
11: 42 31
5: 1 14 | 15 1
19: 14 1 | 14 14
12: 24 14 | 19 1
16: 15 1 | 14 14
31: 14 17 | 1 13
6: 14 14 | 1 14
2: 1 24 | 14 4
0: 8 11
13: 14 3 | 1 12
15: 1 | 14
17: 14 2 | 1 7
23: 25 1 | 22 14
28: 16 1
4: 1 1
20: 14 14 | 1 15
3: 5 14 | 16 1
27: 1 6 | 14 18
14: "b"
21: 14 1 | 1 14
25: 1 1 | 1 14
22: 14 14
8: 42
26: 14 22 | 1 20
18: 15 15
7: 14 5 | 1 21
24: 14 1

abbbbbabbbaaaababbaabbbbabababbbabbbbbbabaaaa
bbabbbbaabaabba
babbbbaabbbbbabbbbbbaabaaabaaa
aaabbbbbbaaaabaababaabababbabaaabbababababaaa
bbbbbbbaaaabbbbaaabbabaaa
bbbababbbbaaaaaaaabbababaaababaabab
ababaaaaaabaaab
ababaaaaabbbaba
baabbaaaabbaaaababbaababb
abbbbabbbbaaaababbbbbbaaaababb
aaaaabbaabaaaaababaa
aaaabbaaaabbaaa
aaaabbaabbaaaaaaabbbabbbaaabbaabaaa
babaaabbbaaabaababbaabababaaab
aabbbbbaabbbaaaaaabbbbbababaaaaabbaaabba)";

    auto [rules, messages] = read_input(std::move(ss));

    day19::rule r8;
    r8.type     = day19::rule::TYPE::SUBRULE;
    r8.subrules = std::vector{std::vector{42}, std::vector{42, 8}};

    day19::rule r11;
    r11.type     = day19::rule::TYPE::SUBRULE;
    r11.subrules = std::vector{std::vector{42, 31}, std::vector{42, 11, 31}};

    rules[8]  = r8;
    rules[11] = r11;

    REQUIRE_FALSE(match(rules, rules[0], messages[11]));
    REQUIRE(match(rules, rules[0], messages[14]));
}

TEST_CASE("Can match string")
{
    std::stringstream ss;

    ss << R"(0: 4 1 5
1: 2 3 | 3 2
2: 4 4 | 5 5
3: 4 5 | 5 4
4: "a"
5: "b"

ababbb
bababa
abbbab
aaabbb
aaaabbb)";

    auto [rules, messages] = read_input(std::move(ss));

    REQUIRE(match(rules, rules[0], messages[0]));
    REQUIRE_FALSE(match(rules, rules[0], messages[1]));
    REQUIRE(match(rules, rules[0], messages[2]));
    REQUIRE_FALSE(match(rules, rules[0], messages[3]));
    REQUIRE_FALSE(match(rules, rules[0], messages[4]));
}

TEST_CASE("Can read input")
{
    std::stringstream ss;

    ss << R"(0: 4 1 5
1: 2 3 | 3 2
2: 4 4 | 5 5
3: 4 5 | 5 4
4: "a"
5: "b"

ababbb
bababa
abbbab
aaabbb
aaaabbb)";

    auto [rules, messages] = read_input(std::move(ss));

    REQUIRE(6 == rules.size());
    REQUIRE(5 == messages.size());

    REQUIRE(day19::rule::TYPE::SUBRULE == rules[0].type);
    REQUIRE(std::vector{4, 1, 5} == rules[0].subrules[0]);
    REQUIRE(std::vector{2, 3} == rules[1].subrules[0]);
    REQUIRE(std::vector{3, 2} == rules[1].subrules[1]);

    REQUIRE(day19::rule::TYPE::MATCH == rules[4].type);
    REQUIRE('a' == rules[4].match);

    REQUIRE("bababa" == messages[1]);
}

TEST_CASE("Can solve part 1 example")
{
    std::stringstream ss;

    ss << R"(0: 4 1 5
1: 2 3 | 3 2
2: 4 4 | 5 5
3: 4 5 | 5 4
4: "a"
5: "b"

ababbb
bababa
abbbab
aaabbb
aaaabbb)";

    auto [rules, messages] = read_input(std::move(ss));

    REQUIRE(2 == part1(rules, messages));
}

TEST_CASE("Can solve part 2 example")
{
    std::stringstream ss;

    ss << R"(42: 9 14 | 10 1
9: 14 27 | 1 26
10: 23 14 | 28 1
1: "a"
11: 42 31
5: 1 14 | 15 1
19: 14 1 | 14 14
12: 24 14 | 19 1
16: 15 1 | 14 14
31: 14 17 | 1 13
6: 14 14 | 1 14
2: 1 24 | 14 4
0: 8 11
13: 14 3 | 1 12
15: 1 | 14
17: 14 2 | 1 7
23: 25 1 | 22 14
28: 16 1
4: 1 1
20: 14 14 | 1 15
3: 5 14 | 16 1
27: 1 6 | 14 18
14: "b"
21: 14 1 | 1 14
25: 1 1 | 1 14
22: 14 14
8: 42
26: 14 22 | 1 20
18: 15 15
7: 14 5 | 1 21
24: 14 1

abbbbbabbbaaaababbaabbbbabababbbabbbbbbabaaaa
bbabbbbaabaabba
babbbbaabbbbbabbbbbbaabaaabaaa
aaabbbbbbaaaabaababaabababbabaaabbababababaaa
bbbbbbbaaaabbbbaaabbabaaa
bbbababbbbaaaaaaaabbababaaababaabab
ababaaaaaabaaab
ababaaaaabbbaba
baabbaaaabbaaaababbaababb
abbbbabbbbaaaababbbbbbaaaababb
aaaaabbaabaaaaababaa
aaaabbaaaabbaaa
aaaabbaabbaaaaaaabbbabbbaaabbaabaaa
babaaabbbaaabaababbaabababaaab
aabbbbbaabbbaaaaaabbbbbababaaaaabbaaabba)";

    auto [rules, messages] = read_input(std::move(ss));

    REQUIRE(12 == part2(rules, messages));
}

#endif
