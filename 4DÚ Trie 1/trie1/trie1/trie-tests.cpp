#include "trie.hpp"

#include "tests-helpers.hpp"
#include "catch.hpp"

 
TEST_CASE("Basics: inserts") {
    trie trie;
    init(trie);
    scope_guard sg([&] () { deallocate(trie); });

    SECTION("Empty trie") {
        REQUIRE_FALSE(contains(trie, ""));
        REQUIRE(size(trie) == 0);
        REQUIRE(empty(trie));
    }

    SECTION("Trivial inserts") {
        REQUIRE(insert(trie, "abcd"));
        REQUIRE(contains(trie, "abcd"));
        REQUIRE_FALSE(contains(trie, ""));
        REQUIRE_FALSE(contains(trie, "a"));
        REQUIRE_FALSE(contains(trie, "ab"));
        REQUIRE_FALSE(contains(trie, "abc"));
        REQUIRE(size(trie) == 1);

        REQUIRE(insert(trie, "abc"));
        REQUIRE(contains(trie, "abc"));
        REQUIRE(size(trie) == 2);

        REQUIRE_FALSE(empty(trie));
    }

    SECTION("Insert all") {
        insert_all(trie, {"abc", "bc", "a", "bc", "d", "", "d", "abcd", "abc"});
        REQUIRE(size(trie) == 6);
        REQUIRE(contains(trie, ""));
        REQUIRE(contains(trie, "a"));
        REQUIRE(contains(trie, "d"));
        REQUIRE_FALSE(contains(trie, "b"));
        REQUIRE(contains(trie, "bc"));
        REQUIRE_FALSE(contains(trie, "ab"));
        REQUIRE(contains(trie, "abc"));
        REQUIRE(contains(trie, "abcd"));
    }
}

TEST_CASE("Basics: erase") {
    trie trie;
    init(trie);
    scope_guard sg([&] () { deallocate(trie); });

    SECTION("Trivial erase") {
        REQUIRE(insert(trie, "a"));
        REQUIRE(erase(trie, "a"));
        REQUIRE_FALSE(contains(trie, "a"));
        REQUIRE(size(trie) == 0);
        REQUIRE(empty(trie));

        REQUIRE(insert(trie, "bcd"));
        REQUIRE(erase(trie, "bcd"));
        REQUIRE_FALSE(contains(trie, "bcd"));
        REQUIRE(size(trie) == 0);
        REQUIRE(empty(trie));
    }

    SECTION("Erase in the middle") {
        insert_all(trie, {"", "a", "ab", "abc", "abcd"});
        REQUIRE(erase(trie, "ab"));
        REQUIRE(size(trie) == 4);
        REQUIRE_FALSE(contains(trie, "ab"));

        REQUIRE(erase(trie, "abc"));
        REQUIRE(size(trie) == 3);
        REQUIRE_FALSE(contains(trie, "abc"));
        
        REQUIRE(contains(trie, "abcd"));
    }
}

TEST_CASE("Complex: inserts") {
    trie trie;
    init(trie);
    scope_guard sg([&] () { deallocate(trie); });

    SECTION("Repeated inserts do not reinsert given string") {
        REQUIRE(insert(trie, "abcd"));
        REQUIRE_FALSE(insert(trie, "abcd"));
        REQUIRE(insert(trie, "abc"));
        REQUIRE_FALSE(insert(trie, "abc"));

        REQUIRE(contains(trie, "abcd"));
        REQUIRE(contains(trie, "abc"));
        REQUIRE(size(trie) == 2);
    }
}

TEST_CASE("Complex: repeated interaction") {
    trie trie;
    init(trie);
    scope_guard sg([&] () { deallocate(trie); });

    SECTION("Reinserting and reerasing the same element works properly") {
        insert_all(trie, { "a", "aa", "ab", "ac" });
        REQUIRE(erase(trie, "a"));
        REQUIRE_FALSE(contains(trie, "a"));
        REQUIRE(insert(trie, "a"));
        REQUIRE(contains(trie, "a"));
        REQUIRE(erase(trie, "a"));
        REQUIRE_FALSE(erase(trie, "a"));
        REQUIRE(contains(trie, "aa"));
        REQUIRE(contains(trie, "ab"));
        REQUIRE(contains(trie, "ac"));

        REQUIRE(size(trie) == 3);
    }
}
