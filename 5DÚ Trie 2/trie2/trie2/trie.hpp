#include <vector>
#include <string>
#include <iterator>

// Assume only basic ASCII characters
static const size_t num_chars = 128;

struct trie_node {
    trie_node* children[num_chars] = {};
    trie_node* parent = nullptr;
    char payload = 0;
    bool is_terminal = false;
};

/*
 * word_cursor encapsulates iteration over words contained in a trie
 */
class word_cursor {
    const trie_node* m_ptr = nullptr;
public:
    word_cursor(const trie_node* word_ptr = nullptr);
    /*
     * Returns true iff this cursor can provide another word via read_word.
     */
    bool has_word() const;
    /*
     * Returns the word this cursor is pointing at.
     *
     * Precondition: has_word returns true given current state
     */
    std::string read_word() const;
    /*
     * Moves cursor to the next word. If no word is available, leaves word_cursor
     * in a state where has_word returns false.
     */
    void move_to_next_word();
};

class trie {
public:
    /**
     * Constructs trie containing all strings from provided vector
     */
    trie(const std::vector<std::string>& strings);

    trie();
	trie(const trie& rhs) = delete;
	trie& operator=(const trie& rhs) = delete;
	trie(trie&& rhs) = delete;
	trie& operator=(trie&& rhs) = delete;
    ~trie();

    /**
     * Removes given string from the trie
     * Returns true iff string was removed (it was present in the trie).
     */
    bool erase(const std::string& str);

    /**
     * Inserts given string to the trie.
     * Returns true iff string was successfully inserted (it was not present before).
     */
    bool insert(const std::string& str);

    /**
     * Returns true iff given string is in the trie
     */
    bool contains(const std::string& str) const;

    /**
     * Returns how many unique strings are in the trie
     */
    size_t size() const;

    /**
     * Returns whether given trie is empty (contains no strings)
     */
    bool empty() const;

    /**
     * Returns all strings from trie that contain given prefix.
     *
     * Prefix can be inclusive, meaning that a prefix of "abc" should return
     * "abc" amongst results, if it is in the trie.
     */
    std::vector<std::string> search_by_prefix(const std::string& prefix) const;

    /**
     * Returns all strings from trie that are prefixes of given string.
     *
     * Prefixes are inclusive, meaning that a string of "abc" should return
     * "abc" amongst results, if it is in the trie.
     */
    std::vector<std::string> get_prefixes(const std::string& str) const;

    word_cursor get_word_cursor() const;
private:
    trie_node* m_root = nullptr;
    size_t m_size = 0;
};
