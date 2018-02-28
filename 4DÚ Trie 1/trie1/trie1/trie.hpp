#pragma once

#include <string>
#include <vector>

// Assume only basic ASCII characters
static const size_t num_chars = 128;

struct trie_node {
    trie_node* children[num_chars] = {};
    trie_node* parent = nullptr;
    char payload = 0;
    bool is_terminal = false;
};

struct trie {
    trie_node* root = nullptr;
    size_t size = 0;
};

/**
 * Initialize trie to default state. Will always be called before further manipulation
 */
void init(trie& trie);

/**
 * Removes given string from the trie
 * Returns true iff string was removed (it was present in the trie).
 */
bool erase(trie& trie, const std::string& str);

/**
 * Deallocates all nodes of given trie.
 * The trie will not be used again afterwards and can be left in an arbitrary state
 */
void deallocate(trie& trie);

/**
 * Inserts given string to the trie.
 * Returns true iff string was successfully inserted (it was not present before).
 */
bool insert(trie& trie, const std::string& str);

/**
 * Returns true iff given string is in the trie
 */
bool contains(const trie& trie, const std::string& str);

/**
 * Inserts all strings from vector to the trie
 */
void insert_all(trie& trie, const std::vector<std::string>& items);

/**
 * Returns how many unique strings are in the trie
 */
size_t size(const trie& trie);

/**
 * Returns whether given trie is empty (contains no strings)
 */
bool empty(const trie& trie);
