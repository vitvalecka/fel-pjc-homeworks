#include "catch.hpp"
#include "expr.hpp"
#include "tests-small6-data.hpp"

#include <tuple>

test_data basic_tests = {
    {"1 + 2", 3},
    {"2 * 2", 4},
    {"1 / 2", 0.5},
    {"1 / 2 * 3", 1.5},
    {"3 * 2 / 4", 1.5},
    {"1 - 2", - 1},
    {"1 + 1 - 1 + 1 - 1 - 1", 0},
    {"0 * 3", 0},
    {"3 ^ 0", 1},
    {"3 ^ 3", 27},
};

TEST_CASE("Basics") {
    for (const auto& test : basic_tests) {
        std::string input;
        double expected;
        std::tie(input, expected) = test;
        INFO("For input: " << input);
        auto e = create_expression_tree(input);
        INFO("Got expression: " << *e);
        REQUIRE(e->evaluate() == Approx(expected));
    }
}


TEST_CASE("Edge cases") {
    std::string input;
    double expected;
    SECTION("Singular values") {
        input = "1.23";
        expected = 1.23;
        REQUIRE(create_expression_tree(input)->evaluate() == Approx(expected));
        input = "(1.23)";
        expected = 1.23;
        REQUIRE(create_expression_tree(input)->evaluate() == Approx(expected));
    }
    SECTION("Overused parens") {
        input = "((1.12)+(2.23))";
        expected = 3.35;
        REQUIRE(create_expression_tree(input)->evaluate() == Approx(expected));
        input = "(((1.12)+(2.23))-(2.3))";
        expected = 1.05;
        REQUIRE(create_expression_tree(input)->evaluate() == Approx(expected));
    }
    SECTION("Associativity") {
        input = "2 ^ 3 ^ 2";
        expected = 512;
        REQUIRE(create_expression_tree(input)->evaluate() == Approx(expected));
        input = "(2 ^ 3) ^ 2";
        expected = 64;
        REQUIRE(create_expression_tree(input)->evaluate() == Approx(expected));
    }
}


TEST_CASE("Single operator expressions") {
    for (const auto& test : simple_tests) {
        std::string input;
        double expected;
        for (const auto& c : test) {
            std::tie(input, expected) = c;
            INFO("For input: " << input);
            auto e = create_expression_tree(input);
            INFO("Got expression: " << *e);
            REQUIRE(e->evaluate() == Approx(expected));
        }
    }
}


TEST_CASE("Complex Expressions") {
    std::string input;
    double expected;
    for (const auto& c : complex_tests) {
        std::tie(input, expected) = c;
        INFO("For input: " << input);
        auto e = create_expression_tree(input);
        INFO("Got expression: " << *e);
        REQUIRE(e->evaluate() == Approx(expected));
    }
}
