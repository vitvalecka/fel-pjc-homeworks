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

    void insert_all(trie& t, const std::vector<std::string>& strings) {
        for (const auto& string : strings) {
            t.insert(string);
        }
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

TEST_CASE("Cursor") {
    trie t({ "abcd", "aceg", "adgj", "bcde" });
    auto cursor = t.get_word_cursor();
    SECTION("Prefix increment") {
        REQUIRE(cursor.has_word());
        REQUIRE(cursor.read_word() == "abcd");
        cursor.move_to_next_word();

        REQUIRE(cursor.has_word());
        REQUIRE(cursor.read_word() == "aceg");
        cursor.move_to_next_word();

        REQUIRE(cursor.has_word());
        REQUIRE(cursor.read_word() == "adgj");
        cursor.move_to_next_word();

        REQUIRE(cursor.has_word());
        REQUIRE(cursor.read_word() == "bcde");
        cursor.move_to_next_word();

        REQUIRE_FALSE(cursor.has_word());
    }
    SECTION("Cursor can be reused") {
        auto cursor2 = cursor;
        cursor.move_to_next_word();
        REQUIRE(cursor2.read_word() == "abcd");
        cursor.move_to_next_word();
        REQUIRE(cursor2.read_word() == "abcd");
        cursor.move_to_next_word();
        REQUIRE(cursor2.read_word() == "abcd");
        cursor2.move_to_next_word();
        REQUIRE(cursor2.read_word() == "aceg");
    }
    SECTION("Empty string handling") {
        trie yes({ "", "a", "b", "aaa", "aab" });
        auto cursor = yes.get_word_cursor();
        REQUIRE(cursor.read_word() == "");
        trie no({ "a", "b", "aaa", "aab" });
        cursor = no.get_word_cursor();
        REQUIRE(cursor.read_word() == "a");
    }
}

TEST_CASE("Search by prefix") {
    trie trie;
    auto elems = std::vector<std::string>{ "a", "aa", "aaa", "aabb", "aabab", "aaaab", "aaqqq" };
    insert_all(trie, elems);
    SECTION("Search is inclusive") {
        VALIDATE_SETS(trie.search_by_prefix("aa"), as_vec({ "aa", "aaa", "aabb", "aabab", "aaaab", "aaqqq" }));
    }
    SECTION("Prefix not in the trie") {
        VALIDATE_SETS(trie.search_by_prefix(""), elems);
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
