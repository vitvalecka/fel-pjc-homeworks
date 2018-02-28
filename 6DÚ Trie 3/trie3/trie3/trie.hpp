#include <vector>
#include <string>
#include <iterator>
#include <iosfwd>

// Assume only basic ASCII characters
static const size_t num_chars = 128;

struct trie_node {
    trie_node* children[num_chars] = {};
    trie_node* parent = nullptr;
    char payload = 0;
    bool is_terminal = false;
};

class trie {
public:

    class const_iterator {
        const trie_node* current_node = nullptr;
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = std::string;
        using reference = std::string;
        using pointer = std::string;
        using difference_type = std::ptrdiff_t;

        const_iterator() = default;
        const_iterator(const trie_node* node);

        const_iterator& operator++();
        const_iterator operator++(int);

        reference operator*() const;
        bool operator==(const const_iterator& rhs) const;
        bool operator!=(const const_iterator& rhs) const;
    };

    /**
     * Constructs trie containing all strings from provided vector
     */
    trie(const std::vector<std::string>& strings);

    trie();
    trie(const trie& rhs);
    trie& operator=(const trie& rhs);
    trie(trie&& rhs);
    trie& operator=(trie&& rhs);
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

    const_iterator begin() const;
    const_iterator end() const;

    void swap(trie& rhs);

	// Relops

	// 2 tries are equal iff they contain the same strings
	bool operator==(const trie& rhs) const;
	/**
	 * Tries are compared by taking lexicographically sorted strings contained within and then lexicographically comparing these.
	 */
	bool operator<(const trie& rhs) const;

    /**
     * Returns new trie that contains the intersection (strings that are present in both) of the two provided tries.
     */
    trie operator&(trie const& rhs) const;

    /**
     * Returns new trie that contains the union (strings are present in at least one) of the two provided tries.
     */
    trie operator|(trie const& rhs) const;

private:
    trie_node* m_root = nullptr;
    size_t m_size = 0;
};

// 2 tries are unequal iff they contain different strings
bool operator!=(const trie& lhs, const trie& rhs);
// Lexicographical ordering, see operator< for details
bool operator<=(const trie& lhs, const trie& rhs);
// Lexicographical ordering, see operator< for details
bool operator>(const trie& lhs, const trie& rhs);
// Lexicographical ordering, see operator< for details
bool operator>=(const trie& lhs, const trie& rhs);
void swap(trie& lhs, trie& rhs);

std::ostream& operator<<(std::ostream& out, trie const& trie);
