#include "catch.hpp"

#include "small1.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <limits>
#include <random>
#include <iomanip>

namespace {

auto& get_rng() {
    static std::minstd_rand rng{ std::random_device{}() };
    return rng;
}

size_t random_size(size_t from, size_t to) {
    auto& rng = get_rng();
    return std::uniform_int_distribution<size_t>(from, to)(rng);
}

int random_int(int from, int to) {
    auto& rng = get_rng();
    return std::uniform_int_distribution<int>(from, to)(rng);
}

bool random_bool(double p = 0.5) {
    auto& rng = get_rng();
    return std::bernoulli_distribution(p)(rng);
}

double random_prob() {
    static std::uniform_real_distribution<> dist;
    return dist(get_rng());
}

char whitespace[] = {
    '\t', ' ',
};

char random_ws() {
    return whitespace[random_size(0, 1)];
}

struct generated_matrix {
    generated_matrix():width(random_size(5, 100)), height(random_size(5, 100)) {}


    std::stringstream create_stream(bool valid_stream = true) {
        size_t breaking_height = std::numeric_limits<size_t>::max();
        size_t breaking_width;
        if (!valid_stream) {
            // Select a row to cut short
            // -- the selection range is [from, to], but our range is [from, to)
            breaking_height = random_size(0, height - 1);
            // Make sure there is something in the row, and it isn't left full
            breaking_width = random_size(1, width - 2);
        }

        std::stringstream res;
        for (size_t h = 0; h < height; ++h) {
            for (size_t w = 0; w < width; ++w) {
                if (h == breaking_height &&
                    w == breaking_width) {
                    break;
                }
                if (w > 0) {
                    auto amount = random_size(1, 250);
                    for (size_t i = 0; i < amount; ++i) {
                        res << random_ws();
                    }
                }
                res << random_int(-1'000, 1'000);
            }
            res << '\n';
        }
        return res;
    }
    size_t width;
    size_t height;
};

std::string generate_valid_string() {
    bool front = random_bool(0.25);
    std::stringstream result;
    if (front) {
        result << "true ";
        auto num = random_int(0, 1);
        result << std::oct << num << " " << std::dec << num << " " << std::hex << num;
    } else {
        result << "false ";
        auto num1 = random_int(101, 200);
        auto num2 = random_int(  1, 100);
        auto num3 = random_int(201, 300);
        result << std::oct << num1 << " " << std::dec << num2 << " " << std::hex << num3;
    }
    return result.str();
}

std::string generate_broken_string() {
    std::stringstream result;
    double prob = random_prob();

    if (prob < 0.25) {
        result << (random_bool()? "true" : "false");
    }
    if (prob < 0.75) {
        result << ' ' << std::oct << random_int(1, 100);
    }
    if (prob > 0.4) {
        result << ' ' << std::dec << random_int(1, 100);
    }
    if (prob > 0.6) {
        result << ' ' << std::hex << random_int(1, 100);
    }

    return result.str();
}

std::string generate_mismatched_stringf() {
    std::stringstream result("false ");
    auto num = random_int(1, 300);

    result << std::oct << num << " " << std::dec << num << " " << std::hex << num;
    return result.str();
}

std::string generate_mismatched_stringt() {
    std::stringstream result("true ");

    auto num1 = random_int(101, 200);
    auto num2 = random_int(1, 100);
    auto num3 = random_int(201, 300);

    double prob = random_prob();
    if (prob < 0.3) {
        num1 = num3;
    } else if (prob < 0.6) {
        num1 = num2;
    } else {
        num2 = num3;
    }

    result << std::oct << num1 << " " << std::dec << num2 << " " << std::hex << num3;
    return result.str();
}

std::string generate_invalid_string() {
    double prob = random_prob();
    if (prob < 0.2) {
        // invalid string altogether
        return generate_broken_string();
    } else if (prob < 0.6) {
        // invalid true tagged string
        return generate_mismatched_stringt();
    } else {
        // invalid false tagged string
        return generate_mismatched_stringf();
    }
}


}

TEST_CASE("parse_matrix", "[small1]") {

    std::stringstream str;
    str.exceptions(std::ios_base::badbit);

    GIVEN("Easy to parse rectangular matrix") {
        std::string s =
R"f(1 2 3 4
5 6 7 8
9 10 11 12
13 14 15 16
)f";

        str.str(s);

        WHEN("matrix is parsed") {
            auto temp = parse_matrix(str);
            THEN("dimensions are equal and correct") {
                REQUIRE(temp.first == 4);
                REQUIRE(temp.second == 4);
            }
        }
    }

    GIVEN("Singular column matrix") {
        std::string s = "1\n2\n3\n4\n5\n6\n7\n";

        str.str(s);

        WHEN("matrix is parsed") {
            auto temp = parse_matrix(str);
            THEN("dimensions are correct") {
                REQUIRE(temp.first == 7);
                REQUIRE(temp.second == 1);
            }
        }
    }

    GIVEN("Singular row matrix") {
        std::string s = "-1 -2 -3 4 5 6 7\n";

        str.str(s);

        WHEN("matrix is parsed") {
            auto temp = parse_matrix(str);
            THEN("dimensions are correct") {
                REQUIRE(temp.first == 1);
                REQUIRE(temp.second == 7);
            }
        }
    }


    GIVEN("Wide matrix") {
        std::string s = R"f(1 2 3 4 5 6 7 8
1 2 3 4 5 6 7 8
1 2 3 4 5 6 7 8
)f";

        str.str(s);

        WHEN("matrix is parsed") {
            auto temp = parse_matrix(str);
            THEN("dimensions are correct") {
                REQUIRE(temp.first == 3);
                REQUIRE(temp.second == 8);
            }
        }
    }

    GIVEN("Empty string (no matrix)") {
        std::string s = "";
        str.str(s);

        WHEN("matrix is parsed") {
            auto temp = parse_matrix(str);
            THEN("dimensions are equal to zero") {
                REQUIRE(temp.first == 0);
                REQUIRE(temp.second == 0);
            }
        }
    }

    GIVEN("String with single number") {
        std::string s = "11\n";
        str.str(s);

        WHEN("matrix is parsed") {
            auto temp = parse_matrix(str);
            THEN("dimensions are equal to one") {
                REQUIRE(temp.first == 1);
                REQUIRE(temp.second == 1);
            }
        }
    }

    GIVEN("Tall matrix") {
        std::string s = R"f(1 2 3
4 5 6
7 8 9
10 11 12
13 14 15
)f";

        str.str(s);

        WHEN("matrix is parsed") {
            auto temp = parse_matrix(str);
            THEN("dimensions are correct") {
                REQUIRE(temp.first == 5);
                REQUIRE(temp.second == 3);
            }
        }
    }

    GIVEN("Matrix with weird whitespace") {
        std::string s = R"f(1 2		3 4
5		6		7 8
9	10 12 13
13 14		15 33
16 17	22	11
)f";
        str.str(s);

        WHEN("matrix is parsed") {
            auto temp = parse_matrix(str);
            THEN("dimensions are correct") {
                REQUIRE(temp.first == 5);
                REQUIRE(temp.second == 4);
            }
        }
    }

    GIVEN("Matrix with rows that have different width") {
        std::string s = R"f(1 2 3 4
5 6 7
)f";
        str.str(s);

        WHEN("matrix is parsed") {
            THEN("exception is thrown (inconsistent row sizes)") {
                REQUIRE_THROWS_AS(parse_matrix(str), std::invalid_argument);
            }
        }
    }

    GIVEN("Matrix with middle row different") {
        std::string s = R"f(1 2 3 4
4 56 7 8
5 6 7
77 889 97 10
8 9 10 11
)f";
        str.str(s);

        WHEN("matrix is parsed") {
            THEN("exception is thrown (inconsistent row sizes)") {
                REQUIRE_THROWS_AS(parse_matrix(str), std::invalid_argument);
            }
        }
    }

    SECTION("Randomly generated matrices") {
        const int tests = 3;
        SECTION("Valid") {
            for (int i = 0; i < tests; ++i) {
                generated_matrix m;
                auto str = m.create_stream();
                auto temp = parse_matrix(str);
                REQUIRE(temp.first == m.height);
                REQUIRE(temp.second == m.width);
            }
        }
        SECTION("Invalid") {
            for (int i = 0; i < tests; ++i) {
                generated_matrix m;
                auto str = m.create_stream(false);
                REQUIRE_THROWS_AS(parse_matrix(str), std::invalid_argument);
            }
        }
    }
}

TEST_CASE("Randomly generated matrices", "[.long]") {
    const int tests = 100;
    SECTION("Autogenerated") {
        SECTION("Valid") {
            for (int i = 0; i < tests; ++i) {
                generated_matrix m;
                auto str = m.create_stream();
                auto temp = parse_matrix(str);
                REQUIRE(temp.first == m.height);
                REQUIRE(temp.second == m.width);
            }
        }
        SECTION("Invalid") {
            for (int i = 0; i < tests; ++i) {
                generated_matrix m;
                auto str = m.create_stream(false);
                REQUIRE_THROWS_AS(parse_matrix(str), std::invalid_argument);
            }
        }
    }
}

TEST_CASE("print_table", "[small1]") {
    std::stringstream str;

    SECTION("input 1") {
        std::vector<std::string> vec = {"abc", "def", "ghi", "jkl"};
        print_table(str, vec);
        std::string exp =
R"f(-----------
| abc| def|
| ghi| jkl|
-----------
)f";
        REQUIRE(str.str() == exp);
    }

    SECTION("input 2") {
        std::vector<std::string> vec = {"123", "456", "789", "012"};
        print_table(str, vec);
        std::string exp =
R"f(-----------
| 123| 456|
| 789| 012|
-----------
)f";
        REQUIRE(str.str() == exp);
    }

    SECTION("input 3") {
        std::vector<std::string> vec = {"abcd", "def", "ghi", "jkl"};
        print_table(str, vec);
        std::string exp =
R"f(-------------
| abcd|  def|
|  ghi|  jkl|
-------------
)f";
        REQUIRE(str.str() == exp);
    }

    SECTION("input 4") {
        std::vector<std::string> vec = {"abcd", "def", "ghi"};
        print_table(str, vec);
        std::string exp =
R"f(-------------
| abcd|  def|
|  ghi|     |
-------------
)f";
        REQUIRE(str.str() == exp);
    }

    SECTION("input 5") {
        std::vector<std::string> vec = {
            "James", "David", "Christopher", "George", "Ronald", "John", "Richard",
            "Daniel", "Kenneth", "Anthony", "Robert", "Charles", "Paul", "Steven",
            "Kevin", "Michael", "Joseph", "Mark", "Edward", "Jason", "William", "Thomas",
            "Donald", "Brian"
        };
        print_table(str, vec);
        std::string exp =
R"f(---------------------------
|       James|       David|
| Christopher|      George|
|      Ronald|        John|
|     Richard|      Daniel|
|     Kenneth|     Anthony|
|      Robert|     Charles|
|        Paul|      Steven|
|       Kevin|     Michael|
|      Joseph|        Mark|
|      Edward|       Jason|
|     William|      Thomas|
|      Donald|       Brian|
---------------------------
)f";
        REQUIRE(str.str() == exp);
    }
}

TEST_CASE("Line validation", "[small1]") {

    GIVEN("properly tagged equal numbers") {
        CHECK(validate_line("true 2 2 2"));
    }

    GIVEN("improperly tagged equal string") {
        CHECK(!validate_line("false 2 2 2"));
    }

    GIVEN("properly tagged non-equal string") {
        CHECK(validate_line("false 2 3 3"));
    }

    GIVEN("improperly tagged non-equal string") {
        CHECK(!validate_line("true 2 3 3"));
    }

    GIVEN("complex properly tagged equal string") {
        CHECK(validate_line("true 0111 73 0x49"));
    }

    GIVEN("complex improperly tagged equal string") {
        CHECK(!validate_line("false 0111 73 0x49"));
    }

    GIVEN("complex properly tagged non-equal string") {
        CHECK(validate_line("false 0131 74 0x4A"));
    }

    GIVEN("complex improperly tagged non-equal string") {
        CHECK(!validate_line("false 07756 4078 0xFEE"));
    }

    GIVEN("string not conforming to format -- missing values") {
        CHECK(!validate_line("false 112"));
    }

    GIVEN("string not conforming to format -- bad number format") {
        CHECK(!validate_line("false 88 88 0x58"));
    }

    GIVEN("string not conforming to format -- not numbers") {
        CHECK(!validate_line("false 07 7 G"));
    }

    GIVEN("properly tagged equal numbers with garbage at the end") {
        CHECK(!validate_line("true 2 2 2x"));
    }

    SECTION("Randomly generated lines") {
        const int tests = 25;
        SECTION("Valid lines") {
            for (int i = 0; i < tests; ++i) {
                auto str = generate_valid_string();
                INFO("For line " << str);
                CHECK(validate_line(str));
            }
        }
        SECTION("Invalid lines") {
            for (int i = 0; i < tests; ++i) {
                auto str = generate_invalid_string();
                INFO("For line " << str);
                CHECK(!validate_line(str));
            }
        }
    }
}

TEST_CASE("Randomly generated lines", "[.long]") {
    const int tests = 1000;
    SECTION("Valid lines") {
        for (int i = 0; i < tests; ++i) {
            auto str = generate_valid_string();
            INFO("For line " << str);
            CHECK(validate_line(str));
        }
    }
    SECTION("Invalid lines") {
        for (int i = 0; i < tests; ++i) {
            auto str = generate_invalid_string();
            INFO("For line " << str);
            CHECK(!validate_line(str));
        }
    }
}

TEST_CASE("Number comparison", "[small1]") {
    std::stringstream sstream;
    GIVEN("Empty stream") {
        CHECK(max_number(sstream) == "");
    }
    GIVEN("Single number in stream") {
        sstream << "123";
        CHECK(max_number(sstream) == "123");
    }
    GIVEN("Same length numbers in stream") {
        sstream << "123,345,677,231";
        CHECK(max_number(sstream) == "677");
    }

    GIVEN("Different length numbers in stream") {
        sstream << "1,22,333,444,4444";
        CHECK(max_number(sstream) == "4444");
    }

    GIVEN("Same length looong numbers") {
        sstream << "1361129467683753853853498429727072845820,"
                << "1361129467683753853853498429727072845821,"
                << "1361129467683753853853498429727072845822,"
                << "1361129467683753853853498429727072845823,"
                << "1361129467683753853853498429727072845824,"
                << "1361129467683753853853498429727072845825,"
                << "1361129467683753853853498429727072845826,"
                << "1361129467683753853853498429727072845827,"
                << "1361129467683753853853498429727072845828,"
                << "1361129467683753853853498429727072845829,"
                << "1361129467683753853853498429727072845828,"
                << "1361129467683753853853498429727072845827,"
                << "1361129467683753853853498429727072845826,"
                << "1361129467683753853853498429727072845825,"
                << "1361129467683753853853498429727072845824,"
                << "1361129467683753853853498429727072845823,"
                << "1361129467683753853853498429727072845822,"
                << "1361129467683753853853498429727072845821,"
                << "1361129467683753853853498429727072845820";
        CHECK(max_number(sstream) == "1361129467683753853853498429727072845829");
    }

    GIVEN("Different length long numbers") {
        sstream << "123456789098765432123456789,"
                << "12345678909876543212345678,"
                << "9234567890987654321234567,"
                << "1234567890987654321234567890";
        CHECK(max_number(sstream) == "1234567890987654321234567890");
    }
}
