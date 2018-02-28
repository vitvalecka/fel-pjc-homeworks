#include "trie.hpp"

#include "catch.hpp"

#include <chrono>
#include <random>
#include <iostream>
#include <algorithm>

#define VALIDATE_SETS(lhs, rhs) \
    do {\
        auto l = lhs; \
        auto r = rhs; \
        std::sort(begin(l), end(l)); \
        std::sort(begin(r), end(r)); \
        REQUIRE(l == r);             \
   } while(0)


namespace {

    std::vector<std::string> as_vec(std::vector<std::string> v) {
        return v;
    }

    std::vector<std::string> extract_all(const trie& t) {
        return { t.begin(), t.end() };
    }

    void insert_all(trie& t, const std::vector<std::string>& strings) {
        for (const auto& string : strings) {
            t.insert(string);
        }
    }

    char generate_char() {
        // Note: The proper way to init the generator would be to use a std::random_device
        //       but that would lead to every run being different and I would prefer to keep
        //       the tests deterministic.
        static std::mt19937 gen;
        char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUWXYZ0123456789,.:;/\\\"'";
        // -1 because the dist is [a, b], -1 because there is a \0 at the end
        static std::uniform_int_distribution<std::size_t> dist(0, sizeof(chars) - 2);
        return chars[dist(gen)];
    }

    std::string generate_string() {
        std::string ret;
        const size_t sz = 20;
        ret.reserve(sz);
        std::generate_n(std::back_inserter(ret), sz, generate_char);
        return ret;
    }

    std::vector<std::string> generate_data(size_t sz) {
        std::vector<std::string> res;
        res.reserve(sz);
        std::generate_n(std::back_inserter(res), sz, generate_string);
        return res;
    }
}

TEST_CASE("Basics: inserts") {
    trie trie;

    SECTION("Empty trie") {
        REQUIRE_FALSE(trie.contains(""));
        REQUIRE(trie.size() == 0);
        REQUIRE(trie.empty());
    }

    SECTION("Trivial inserts") {
        REQUIRE(trie.insert("abcd"));
        REQUIRE(trie.contains("abcd"));
        REQUIRE_FALSE(trie.contains(""));
        REQUIRE_FALSE(trie.contains("a"));
        REQUIRE_FALSE(trie.contains("ab"));
        REQUIRE_FALSE(trie.contains("abc"));
        REQUIRE(trie.size() == 1);

        REQUIRE(trie.insert("abc"));
        REQUIRE(trie.contains("abc"));
        REQUIRE(trie.size() == 2);

        REQUIRE_FALSE(trie.empty());
    }
}

TEST_CASE("Basics: erase") {
    trie trie;

    SECTION("Trivial erase") {
        REQUIRE(trie.insert("a"));
        REQUIRE(trie.erase("a"));
        REQUIRE_FALSE(trie.contains("a"));
        REQUIRE(trie.size() == 0);
        REQUIRE(trie.empty());

        REQUIRE(trie.insert("bcd"));
        REQUIRE(trie.erase("bcd"));
        REQUIRE_FALSE(trie.contains("bcd"));
        REQUIRE(trie.size() == 0);
        REQUIRE(trie.empty());
    }

    SECTION("Erase in the middle of a link") {
        insert_all(trie, { "", "a", "ab", "abc", "abcd" });
        REQUIRE(trie.erase("ab"));
        REQUIRE(trie.size() == 4);
        REQUIRE_FALSE(trie.contains("ab"));

        REQUIRE(trie.erase("abc"));
        REQUIRE(trie.size() == 3);
        REQUIRE_FALSE(trie.contains("abc"));

        REQUIRE(trie.contains("abcd"));
    }
}

TEST_CASE("Complex: inserts") {
    trie trie;

    SECTION("Repeated inserts do not reinsert given string") {
        REQUIRE(trie.insert("abcd"));
        REQUIRE_FALSE(trie.insert("abcd"));
        REQUIRE(trie.insert("abc"));
        REQUIRE_FALSE(trie.insert("abc"));

        REQUIRE(trie.contains("abcd"));
        REQUIRE(trie.contains("abc"));
        REQUIRE(trie.size() == 2);
    }
}

TEST_CASE("Complex: repeated interaction") {
    trie trie;

    SECTION("Reinserting and reerasing the same element works properly") {
        insert_all(trie, { "a", "aa", "ab", "ac" });
        REQUIRE(trie.erase("a"));
        REQUIRE_FALSE(trie.contains("a"));
        REQUIRE(trie.insert("a"));
        REQUIRE(trie.contains("a"));
        REQUIRE(trie.erase("a"));
        REQUIRE_FALSE(trie.erase("a"));
        REQUIRE(trie.contains("aa"));
        REQUIRE(trie.contains("ab"));
        REQUIRE(trie.contains("ac"));

        REQUIRE(trie.size() == 3);
    }
}

TEST_CASE("Vector constructor") {
    trie trie({ "abc", "bc", "a", "bc", "d", "", "d", "abcd", "abc" });
    REQUIRE(trie.size() == 6);
    REQUIRE(trie.contains(""));
    REQUIRE(trie.contains("a"));
    REQUIRE(trie.contains("d"));
    REQUIRE_FALSE(trie.contains("b"));
    REQUIRE(trie.contains("bc"));
    REQUIRE_FALSE(trie.contains("ab"));
    REQUIRE(trie.contains("abc"));
    REQUIRE(trie.contains("abcd"));
}

TEST_CASE("Search by prefix") {
    trie trie;
    insert_all(trie, { "a", "aa", "aaa", "aabb", "aabab", "aaaab", "aaqqq" });
    SECTION("Search is inclusive") {
        VALIDATE_SETS(trie.search_by_prefix("aa"), as_vec({ "aa", "aaa", "aabb", "aabab", "aaaab", "aaqqq" }));
    }
    SECTION("Prefix not in the trie") {
        VALIDATE_SETS(trie.search_by_prefix(""), extract_all(trie));
    }
}

TEST_CASE("Get prefixes") {
    trie trie;
    insert_all(trie, { "a", "aa", "aaa", "aabb", "aabab", "aaaab", "aaqqq" });
    SECTION("Search is inclusive") {
        VALIDATE_SETS(trie.get_prefixes("aabab"), as_vec({ "aa", "a", "aabab" }));
    }
    SECTION("Input is not in the trie") {
        VALIDATE_SETS(trie.get_prefixes("aaaaa"), as_vec({ "a", "aa", "aaa" }));
    }
}

TEST_CASE("Iterator") {
    SECTION("Default constructed iterators are equal") {
        trie::const_iterator iter1, iter2;
        REQUIRE(iter1 == iter2);
    }
    trie t({ "abc", "bcd", "cde", "edf" });
    auto it = t.begin();
    auto et = t.end();
    SECTION("End iterator and default constructed iterator are equal") {
        trie::const_iterator iter;
        REQUIRE(iter == et);
    }
    SECTION("Prefix increment") {
        REQUIRE(it != et);
        REQUIRE(*it == "abc");
        ++it;
        REQUIRE(it != et);
        REQUIRE(*it == "bcd");
        ++it;

        REQUIRE(it != et);
        REQUIRE(*it == "cde");
        ++it;

        REQUIRE(it != et);
        REQUIRE(*it == "edf");
        ++it;

        REQUIRE(it == et);
    }
    SECTION("Postfix increment") {
        REQUIRE(it != et);
        REQUIRE(*it == "abc");
        it++;
        REQUIRE(it != et);
        REQUIRE(*it == "bcd");
        it++;

        REQUIRE(it != et);
        REQUIRE(*it == "cde");
        it++;

        REQUIRE(it != et);
        REQUIRE(*it == "edf");
        it++;

        REQUIRE(it == et);
    }
    SECTION("Iterator can be reused") {
        auto it2 = it;
        ++it;
        REQUIRE(*it2 == "abc");
        ++it;
        REQUIRE(*it2 == "abc");
        ++it;
        REQUIRE(*it2 == "abc");
        ++it2;
        REQUIRE(*it2 == "bcd");
    }
    SECTION("Empty string handling") {
        trie yes({ "", "a", "b", "aaa", "aab" });
        auto it = yes.begin();
        REQUIRE(*it == "");
        trie no({ "a", "b", "aaa", "aab" });
        it = no.begin();
        REQUIRE(*it == "a");
    }
}

TEST_CASE("Copy") {
    SECTION("Copy constructor") {
        SECTION("Verify that deep copy is performed") {
            trie t1({ "a", "aa", "ab", "ad", "cd", "c", "q"});
            trie t2(t1);
            SECTION("Add to old trie") {
                t1.insert("xXxXx");
                VALIDATE_SETS(extract_all(t1), as_vec({ "a", "aa", "ab", "ad", "cd", "c", "q", "xXxXx" }));
                VALIDATE_SETS(extract_all(t2), as_vec({ "a", "aa", "ab", "ad", "cd", "c", "q" }));
            }
            SECTION("Add to new trie") {
                t2.insert("xXxXx");
                VALIDATE_SETS(extract_all(t1), as_vec({ "a", "aa", "ab", "ad", "cd", "c", "q" }));
                VALIDATE_SETS(extract_all(t2), as_vec({ "a", "aa", "ab", "ad", "cd", "c", "q", "xXxXx" }));
            }
            SECTION("Add to both tries") {
                t1.insert("xXxXx");
                t2.insert("YYY");
                VALIDATE_SETS(extract_all(t1), as_vec({ "a", "aa", "ab", "ad", "cd", "c", "q", "xXxXx" }));
                VALIDATE_SETS(extract_all(t2), as_vec({ "a", "aa", "ab", "ad", "cd", "c", "q", "YYY" }));
            }
        }
        SECTION("Copying an empty trie must work properly") {
            trie t1;
            trie t2(t1);
            REQUIRE(t1.empty());
            REQUIRE(t2.empty());
        }
    }
    SECTION("Copy assignment") {
        trie t1({"a", "b", "c", "d"});
        trie t2({"A", "B", "C", "D"});
        SECTION("Simple assignment") {
            t1 = t2;
            VALIDATE_SETS(extract_all(t1), as_vec({ "A", "B", "C", "D" }));
            VALIDATE_SETS(extract_all(t2), as_vec({ "A", "B", "C", "D" }));
        }
        SECTION("Self assignment") {
            t1 = t1;
            VALIDATE_SETS(extract_all(t1), as_vec({ "a", "b", "c", "d" }));
            REQUIRE(t1.size() == 4);
        }
        SECTION("Repeated assignments") {
            t2 = t1;
            VALIDATE_SETS(extract_all(t2), as_vec({ "a", "b", "c", "d" }));
            t1.insert("A");
            t2 = t1;
            VALIDATE_SETS(extract_all(t2), as_vec({ "a", "b", "c", "d", "A" }));
            t1.insert("B");
            t2 = t1;
            VALIDATE_SETS(extract_all(t2), as_vec({ "a", "b", "c", "d", "A", "B" }));
            t1.insert("C");
            t2 = t1;
            VALIDATE_SETS(extract_all(t2), as_vec({ "a", "b", "c", "d", "A", "B", "C" }));
            t1.insert("D");
            t2 = t1;
            VALIDATE_SETS(extract_all(t2), as_vec({ "a", "b", "c", "d", "A", "B", "C", "D" }));
        }
    }
}

TEST_CASE("Move operations") {
    SECTION("Move constructor") {
        SECTION("From an empty trie") {
            trie t1;
            trie t2(std::move(t1));
            REQUIRE(t2.empty());
        }
        SECTION("From non-empty trie") {
            trie t1({ "a", "b", "c", "d", "e" });
            trie t2(std::move(t1));
            VALIDATE_SETS(extract_all(t2), as_vec({ "a", "b", "c", "d", "e" }));
        }
    }
    SECTION("Move assignment") {
        SECTION("From empty trie") {
            trie t1;
            SECTION("To empty trie") {
                trie t2;
                t2 = std::move(t1);
                REQUIRE(t2.empty());
            }
            SECTION("To non-empty trie") {
                trie t2({"a", "b", "c", "d", "e"});
                t2 = std::move(t1);
                REQUIRE(t2.empty());
            }
        }
        SECTION("From non-empty trie") {
            trie t1({ "ab", "bc", "deg" });
            SECTION("To empty trie") {
                trie t2;
                t2 = std::move(t1);
                VALIDATE_SETS(extract_all(t2), as_vec({ "ab", "bc", "deg" }));
            }
            SECTION("To non-empty trie") {
                trie t2({"kfa", "bfg", "id"});
                t2 = std::move(t1);
                VALIDATE_SETS(extract_all(t2), as_vec({ "ab", "bc", "deg" }));
            }
        }
        SECTION("Back and forth") {
            trie t1({ "abc", "def", "eee" });
            trie t2({ "qwertz", "qwerty", "azerty" });
            t1 = std::move(t2);
            VALIDATE_SETS(extract_all(t1), as_vec({ "qwertz", "qwerty", "azerty" }));

            t2 = std::move(t1);
            VALIDATE_SETS(extract_all(t2), as_vec({ "qwertz", "qwerty", "azerty" }));

            SECTION("Trie can be used after being move assigned-to") {
                REQUIRE(t2.erase("azerty"));
                REQUIRE(t2.insert("qzerty"));
                VALIDATE_SETS(extract_all(t2), as_vec({ "qwertz", "qwerty", "qzerty" }));
            }
        }
    }
}

TEST_CASE("member swap") {
    SECTION("Swapping empty tries leaves both empty") {
        trie t1, t2;
        t1.swap(t2);
        REQUIRE(t1.empty());
        REQUIRE(t1.empty());
    }
    SECTION("Swapping tries actually swaps elements") {
        trie t1({ "azerty", "qwerty", "qwertz" });
        SECTION("Non-empty and empty trie") {
            trie t2;
            t1.swap(t2);
            REQUIRE(t1.empty());
            VALIDATE_SETS(extract_all(t2), as_vec({ "azerty", "qwerty", "qwertz" }));

            SECTION("Roundtrip") {
                t2.swap(t1);
                REQUIRE(t2.empty());
                VALIDATE_SETS(extract_all(t1), as_vec({"azerty", "qwerty", "qwertz"}));
            }
        }
        SECTION("Non-empty and non-empty trie") {
            trie t2({ "qzerty", "querty", "awerty" });
            t1.swap(t2);
            VALIDATE_SETS(extract_all(t1), as_vec({"qzerty", "querty", "awerty"}));
            VALIDATE_SETS(extract_all(t2), as_vec({"azerty", "qwerty", "qwertz"}));

            SECTION("Roundtrip") {
                t1.swap(t2);
                VALIDATE_SETS(extract_all(t1), as_vec({ "azerty", "qwerty", "qwertz" }));
                VALIDATE_SETS(extract_all(t2), as_vec({ "qzerty", "querty", "awerty" }));
            }
        }
    }
}

TEST_CASE("member swap -- complexity", "[.long]") {
    // Swap should take constant time regardless of trie size.
    using namespace std::chrono_literals;
    double last_time = 0;
    auto remove_duplicates = [] (std::vector<std::string> data) {
        std::sort(begin(data), end(data));
        data.erase(std::unique(begin(data), end(data)),
                   end(data));
        return data;
    };
    for (size_t i = 1'000; i <= 128'000; i *= 2) {
        auto lhs_elems = remove_duplicates(generate_data(i));
        trie lhs{ lhs_elems };
        auto rhs_elems = remove_duplicates(generate_data(i));
        trie rhs{ rhs_elems };
        auto start_time = std::chrono::high_resolution_clock::now();
        lhs.swap(rhs);
        auto end_time = std::chrono::high_resolution_clock::now();
        // Add 0.5 ms to even out variability
        auto time_diff = end_time - start_time + 500us;
        auto total_time = static_cast<double>(time_diff.count());
        VALIDATE_SETS(extract_all(lhs), rhs_elems);
        VALIDATE_SETS(extract_all(rhs), lhs_elems);

        if (last_time) {
            // The coefficient is a rough approximation.
            // A linear time coefficient should blow out, constant time
            // jitter should remain inside some reasonable coefficient
            REQUIRE(total_time / last_time <= 1.25);
        }
        last_time = total_time;
        std::cout << "member swap: i = " << i << " total time = " << (time_diff - 500us).count() << '\n';
    }
}

TEST_CASE("Trie union") {
    trie t1, t2;
    SECTION("Empty tries") {
        auto res = t1 | t2;
        CHECK(res.empty());
        REQUIRE(res.size() == 0);
    }
    SECTION("One trie empty") {
        t1 = trie({ "aa", "aab", "aabb", "zzz" });
        SECTION("LHS trie empty") {
            auto res = t2 | t1;
            CHECK(res.size() == 4);
            REQUIRE(res == t1);
        }
        SECTION("RHS trie empty") {
            auto res = t1 | t2;
            CHECK(res.size() == 4);
            REQUIRE(res == t1);
        }
    }
    SECTION("No empty tries") {
        t1 = trie({ "queue", "quiz", "quizzical", "quilt" });
        SECTION("Identical tries") {
            auto res = t1 | t1;
            CHECK(res.size() == 4);
            REQUIRE(res == t1);
        }
        SECTION("Overlapping tries") {
            t2 = trie({"quilt", "queue", "quitter"});
            auto res = t1 | t2;
            CHECK(res.size() == 5);
            REQUIRE(res == trie({"queue", "quiz", "quizzical", "quilt", "quitter"}));
        }
        SECTION("Distinct tries") {
            t2 = trie({"abra", "kadabra","abrakadabra"});
            auto res = t1 | t2;
            CHECK(res.size() == 7);
            REQUIRE(res == trie({"queue", "quiz", "quizzical", "quilt", "abra", "kadabra", "abrakadabra"}));
        }
    }
    SECTION("Chained unions") {
        t1 = trie({"aaa", "bbb", "ccc"});
        t2 = trie({"bbb", "ccc", "ddd"});
        trie t3({"eee", "fff", "ggg"});
        trie t4({"ggg", "hhh", "iii"});
        auto res = t1 | t2 | t3 | t4;
        CHECK(res.size() == 9);
        REQUIRE(res == trie({"aaa", "bbb", "ccc", "ddd", "eee", "fff", "ggg", "hhh" ,"iii"}));
    }
}

TEST_CASE("Trie intersection") {
    trie t1, t2;
    SECTION("Empty tries") {
        auto res = t1 & t2;
        CHECK(res.empty());
        REQUIRE(res.size() == 0);
    }
    SECTION("One trie empty") {
        t1 = trie({ "aa", "aab", "aabb", "zzz" });
        SECTION("LHS trie empty") {
            auto res = t2 & t1;
            CHECK(res.size() == 0);
            REQUIRE(res == t2);
        }
        SECTION("RHS trie empty") {
            auto res = t1 & t2;
            CHECK(res.size() == 0);
            REQUIRE(res == t2);
        }
    }
    SECTION("No empty tries") {
        t1 = trie({ "queue", "quiz", "quizzical", "quilt" });
        SECTION("Identical tries") {
            auto res = t1 & t1;
            CHECK(res.size() == 4);
            REQUIRE(res == t1);
        }
        SECTION("Overlapping tries") {
            t2 = trie({ "quilt", "queue", "quitter" });
            auto res = t1 & t2;
            CHECK(res.size() == 2);
            REQUIRE(res == trie({ "queue", "quilt" }));
        }
        SECTION("Distinct tries") {
            t2 = trie({ "abra", "kadabra","abrakadabra" });
            auto res = t1 & t2;
            CHECK(res.size() == 0);
            REQUIRE(res == trie{});
        }
    }
    SECTION("Chained intersections") {
        t1 = trie({ "aaa", "bbb", "ccc", "ddd" });
        t2 = trie({ "bbb", "ccc", "ddd", "eee" });
        trie t3({ "ccc", "ddd", "eee", "fff" });
        trie t4({ "ddd", "eee", "fff", "ggg" });
        auto res = t1 & t2 & t3 & t4;
        CHECK(res.size() == 1);
        REQUIRE(res == trie({ "ddd" }));
    }
}

TEST_CASE("Trie intersection - complexity", "[.long]") {
    using namespace std::chrono_literals;
    double last_time = 0;
    auto add_prefix = [] (std::vector<std::string> const& strings, std::string const& prefix) {
        std::vector<std::string> results;
        for (auto const& str : strings) {
            results.push_back(prefix + str);
        }
        return results;
    };
    // The idea:
    // Naive implementation of intersection would be to extract all strings
    // in the tries and then perform intersection over vector<string>.
    // Given 2 tries, one with all words prefixed with 'a' and second
    // with all words prefixed with 'z', this would lead to linear complexity
    // However, proper implementation should intersect such tries in constant time
    for (size_t i = 1'000; i <= 64'000; i *= 2) {
        auto lhs_elems = add_prefix(generate_data(i), "aaa");
        trie lhs{ lhs_elems };
        auto rhs_elems = add_prefix(generate_data(i), "zzz");
        trie rhs{ rhs_elems };
        auto start_time = std::chrono::high_resolution_clock::now();
        auto res = lhs & rhs;
        auto end_time = std::chrono::high_resolution_clock::now();
        // Add 0.5 ms to even out variability
        auto time_diff = end_time - start_time + 500us;
        auto total_time = static_cast<double>(time_diff.count());
        REQUIRE(res == trie{});

        if (last_time) {
            // The coefficient is a rough approximation.
            // A linear time coefficient should blow out, constant time
            // jitter should remain inside some reasonable coefficient
            REQUIRE(total_time / last_time <= 1.25);
        }
        last_time = total_time;
        std::cout << "Trie intersection: i = " << i << " total time = " << (time_diff - 500us).count() << '\n';
    }
}
