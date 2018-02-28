#include "trie.hpp"

#include <utility>
#include <algorithm>

using namespace std;


//
// VLASTNÍ FUNKCE A PROMÌNNÉ (TRIE1)


bool insertAsChild(trie_node *subTrie, const string &str)
{
	if (str.empty())
	{
		return false;
	}
	else if (str.size() == 1)
	{
		int i = 0;

		while ((bool)subTrie->children[i] == true)
		{
			if (subTrie->children[i]->payload == str.at(0))
			{
				if (subTrie->children[i]->is_terminal)
				{
					return false;
				}
				else
				{
					subTrie->children[i]->is_terminal = true;
					return true;
				}
			}
			i++;
		}

		// jsme na konci a prvek ještì neexistuje -> vloží ho
		trie_node * newNode = new trie_node;
		newNode->is_terminal = true;
		newNode->parent = subTrie;
		newNode->payload = str.at(0);
		subTrie->children[i] = newNode;
		return true;
	}
	else
	{
		int i = 0;

		while ((bool)subTrie->children[i] == true)
		{
			if (subTrie->children[i]->payload == str.at(0))
			{
				return insertAsChild(subTrie->children[i], str.substr(1, str.size() - 1));
			}
			i++;
		}

		// chybí vìtev se znakem -> vloží chybìjící znak a pokraèuje ve vìtvi
		trie_node * newNode = new trie_node;
		newNode->is_terminal = false;
		newNode->parent = subTrie;
		newNode->payload = str.at(0);
		subTrie->children[i] = newNode;

		return insertAsChild(newNode, str.substr(1, str.size() - 1));
	}
}


bool findInChildren(trie_node * subTrie, const string & str)
{
	if (str.size() == 0)
	{
		return false;
	}
	else if (str.size() == 1)
	{
		int i = 0;

		while ((bool)subTrie->children[i] == true)
		{
			if ((subTrie->children[i]->payload == str.at(0)) && subTrie->children[i]->is_terminal)
			{
				return true;
			}
			i++;
		}

		return false;
	}
	else
	{
		int i = 0;

		while ((bool)subTrie->children[i] == true)
		{
			if (subTrie->children[i]->payload == str.at(0))
			{
				return findInChildren(subTrie->children[i], str.substr(1, str.size() - 1));
			}

			i++;
		}

		return false;
	}
}


bool deleteWordFromChildren(trie_node *subTrie, const string &str)
{
	if (str.empty())
	{
		return false;
	}
	else if (str.size() == 1)
	{
		int i = 0;

		while (subTrie->children[i])
		{
			if (subTrie->children[i]->payload == str.at(0))
			{
				subTrie->children[i]->is_terminal = false;
				return true;
			}

			i++;
		}

		return false;
	}
	else
	{
		int i = 0;

		while (subTrie->children[i])
		{
			if (subTrie->children[i]->payload == str.at(0))
			{
				return deleteWordFromChildren(subTrie->children[i], str.substr(1, str.size() - 1));
			}

			i++;
		}

		return false;
	}
}


//
// VLASTNÍ FUNKCE A PROMÌNNÉ (TRIE2)


vector<string> findMoreWordsByPrefix(vector<string> vector, trie_node * node, string prefix)
{
	if (node->is_terminal)
	{
		vector.push_back(prefix);
	}

	for (int i = 0; i < num_chars; i++)
	{
		if (node->children[i] != nullptr)
		{
			prefix.push_back(node->children[i]->payload);
			vector = findMoreWordsByPrefix(vector, node->children[i], prefix);
			prefix.pop_back();
		}
	}

	return vector;
}


const trie_node * goInside(trie_node * node)
{
	if (node->is_terminal)
	{
		return node;
	}

	for (int i = 0; i < num_chars; i++)
	{
		if (node->children[i] != nullptr)
		{
			if (node->children[i]->is_terminal)
			{
				return node->children[i];
			}

			node = node->children[i];
			i = -1;
		}
	}

	return node;
}


const trie_node * addNextWord(const trie_node * node)
{
	bool next = false;

	for (int i = 0; i < num_chars; i++)
	{
		if (node->children[i] != nullptr)
		{
			node = node->children[i];

			if (node->is_terminal)
			{
				return node;
			}

			i = -1;
		}
	}

	while (node->parent != nullptr)
	{
		for (int i = 0; i < num_chars; i++)
		{
			if (next && (node->parent->children[i] != nullptr))
			{
				return goInside(node->parent->children[i]);
			}

			if (node->parent->children[i] == node)
			{
				next = true;
			}
		}

		node = node->parent;
		next = false;
	}

	return nullptr;
}


//
// VLASTNÍ FUNKCE A PROMÌNNÉ (TRIE3)

void deleteTrie(trie_node * node)
{
	for (int i = 0; i < num_chars; ++i)
	{
		if (node->children[i])
		{
			deleteTrie(node->children[i]);
		}
	}

	if (node != nullptr)
	{
		delete node;
	}
}


trie_node * goToNodeByWord(trie_node * node, string str)
{
	if ((node->payload == str.at(0)) && (str.length() == 1))
	{
		return node;
	}
	
	for (int i = 0; i < str.length(); i++)
	{
		for (int j = 0; j < num_chars; j++)
		{
			if (node->children[j] && node->children[j]->payload == str.at(i))
			{
				node = node->children[j];
			}
		}
	}
	
	return node;
}


void cleanUpTrie(trie * t, trie_node * root, string str)
{
	if (t->search_by_prefix(str).size() == 0)
	{
		trie_node * foo = goToNodeByWord(root, str);
		trie_node * next = foo->parent;
		
		for (int i = 0; i < num_chars; i++)
		{
			if (next->children[i] == foo)
			{
				next->children[i] = nullptr;
				break;
			}
		}

		delete foo;

		str = str.substr(0, str.size() - 1);

		if (str.length() > 0)
		{
			cleanUpTrie(t, root, str);
		}
	}
}


// 
// FUNKCE A PROMÌNNÉ PODLE "TRIE.HPP" - TRIE 3


trie::trie()
{
	m_root = new trie_node();
	m_size = 0;
}


trie::trie(const vector<string>& strings)
{
	m_root = new trie_node();
	m_size = 0;
	
	for (int i = 0; i < strings.size(); i++)
	{
		insert(strings[i]);
	}
}


trie::trie(trie&& rhs)
{
	m_root = new trie_node();
	m_size = 0;
	
	vector<string> listOfWords = rhs.search_by_prefix("");

	if (rhs.m_root->payload == ' ')
	{
		listOfWords.push_back("");
	}

	for (string word : listOfWords)
	{
		insert(word);
	}
}


trie::trie(const trie& rhs)
{
	vector<string> listOfWords = rhs.search_by_prefix("");

	if (rhs.m_root->payload == ' ')
	{
		listOfWords.push_back("");
	}

	m_root = new trie_node();
	m_size = 0;

	for (string word : listOfWords)
	{
		insert(word);
	}
}


trie::~trie()
{
	trie_node * foo = m_root;
	deleteTrie(foo);

	m_size = 0;
	m_root = nullptr;
}


bool trie::insert(const string& str)
{
	if (str.empty() && (m_root->payload != ' '))
	{
		m_root->payload = ' ';
		m_size++;
	}

	if (insertAsChild(m_root, str))
	{
		m_size++;
		return true;
	}

	return false;
}


bool trie::erase(const string& str)
{
	if (findInChildren(m_root, str) && deleteWordFromChildren(m_root, str))
	{
		m_size--;
		cleanUpTrie(this, m_root, str);
		return true;
	}

	return false;


}


bool trie::contains(const string& str) const
{
	if (str.empty() && (m_root->payload == ' '))
	{
		return true;
	}
	return findInChildren(m_root, str);
}


size_t trie::size() const
{
	return m_size;
}


bool trie::empty() const
{
	return m_size == 0;
}


vector<string> trie::search_by_prefix(const string& str) const
{
	vector<string> words = {};
	string phrase = "";

	int i = 0;
	bool found = false;
	trie_node * foo = m_root;

	while (str[i] != '\0')
	{
		for (int j = 0; j < num_chars; j++)
		{
			if (foo->children[j] != nullptr && foo->children[j]->payload == str[i])
			{
				foo = foo->children[j];
				phrase.push_back(str[i]);
				found = true;
				break;
			}
		}

		if (found)
		{
			found = false;
			i++;
		}
		else
		{
			return words;
		}
	}

	return findMoreWordsByPrefix(words, foo, phrase);
}


vector<string> trie::get_prefixes(const string & str) const
{
	vector<string> prefixes;

	for (int i = str.size(); i > 0; i--)
	{
		if (findInChildren(m_root, str.substr(0, i)))
		{
			prefixes.push_back(str.substr(0, i));
		}
	}

	return prefixes;
}


trie::const_iterator trie::begin() const
{
	const trie_node * foo = m_root;

	if (m_root->payload == ' ')
	{
		return const_iterator(m_root);
	}
	else
	{
		for (int i = 0; i < num_chars; i++)
		{
			if (foo->children[i] != nullptr)
			{
				if (foo->children[i]->is_terminal)
				{
					return const_iterator(foo->children[i]);
				}
				
				foo = foo->children[i];
				i = -1;
			}
		}
	}
}


trie::const_iterator trie::end() const
{
    return const_iterator(nullptr);
}


void trie::swap(trie& rhs)
{
	trie_node * fooNode = m_root;
	m_root = rhs.m_root;
	rhs.m_root = fooNode;

	size_t fooSize = m_size;
	m_size = rhs.m_size;
	rhs.m_size = fooSize;
}


void swap(trie& lhs, trie& rhs)
{
	lhs.swap(rhs);
}


trie& trie::operator=(const trie& rhs)
{
	vector<string> listOfWords = rhs.search_by_prefix("");

	if (rhs.m_root->payload == ' ')
	{
		listOfWords.push_back("");
	}

	deleteTrie(m_root);
	m_root = new trie_node();
	m_size = 0;

	for (string word : listOfWords)
	{
		this->insert(word);
	}

	return * this;
}


trie& trie::operator=(trie&& rhs)
{
	deleteTrie(m_root);
	m_root = new trie_node();
	m_size = 0;
	
	vector<string> listOfWords = rhs.search_by_prefix("");

	if (rhs.m_root->payload == ' ')
	{
		listOfWords.push_back("");
	}

	for (string word : listOfWords)
	{
		this->insert(word);
	}

	return *this;
}


bool trie::operator==(const trie& rhs) const
{
    // TODO
	return true;
}


bool trie::operator<(const trie& rhs) const
{
	// TODO
	return false;
}


trie trie::operator&(trie const& rhs) const
{
	// TODO
	return {};
}


trie trie::operator|(trie const& rhs) const
{
	// TODO
	return {};
}


bool operator!=(const trie& lhs, const trie& rhs)
{
	return !(lhs == rhs);
}


bool operator>(const trie& lhs, const trie& rhs)
{
	return rhs < lhs;
}


bool operator<=(const trie& lhs, const trie& rhs)
{
	return !(lhs > rhs);
}


bool operator>=(const trie& lhs, const trie& rhs)
{
	return !(lhs < rhs);
}


ostream& operator<<(ostream& out, trie const& trie)
{
    return out;
}


//
// CONST ITERATOR

trie::const_iterator& trie::const_iterator::operator++()
{
	current_node = addNextWord(current_node);
	return *this;
}


trie::const_iterator trie::const_iterator::operator++(int)
{
	operator++();
	return *this;
}


trie::const_iterator::const_iterator(const trie_node* node)
{
	current_node = node;
}


bool trie::const_iterator::operator==(const trie::const_iterator& rhs) const
{
    return current_node == rhs.current_node;
}


bool trie::const_iterator::operator!=(const trie::const_iterator& rhs) const
{
    return current_node != rhs.current_node;
}


trie::const_iterator::reference trie::const_iterator::operator*() const
{
	const trie_node * foo = current_node;
	string result = "";

	while (foo->parent != nullptr)
	{
		result = foo->payload + result;
		foo = foo->parent;
	}

	return result;
}
