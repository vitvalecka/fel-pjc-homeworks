#include "trie.hpp"

#include <utility>
#include <algorithm>

using namespace std;

//
// VLASTNÍ FUNKCE A PROMÌNNÉ (TRIE1)

vector<trie_node*> list;

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
		list.push_back(newNode);
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
		list.push_back(newNode);
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

		while ((bool)subTrie->children[i] == true)
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

		while ((bool)subTrie->children[i] == true)
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


void deleteAllChildren(trie_node *subTrie)
{
	if (subTrie != nullptr)
	{
		for (int i = 0; i < num_chars; i++)
		{
			if (subTrie->children[i] != nullptr)
			{
				deleteAllChildren(subTrie->children[i]);
			}
		}

		delete subTrie;
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


string getThisWord(trie_node * node, string word)
{
	while (node->parent != nullptr)
	{
		word = node->payload + word;
		node = node->parent;
	}

	return word;
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
// FUNKCE A PROMÌNNÉ PODLE "TRIE.HPP" - TRIE

trie::trie()
{
	m_root = new trie_node;
	m_root->is_terminal = false;
	list.push_back(m_root);
	m_size = 0;
}

trie::trie(const vector<string>& strings)
	:trie()
{
	for (int i = 0; i < strings.size(); i++)
	{
		trie::insert(strings[i]);
	}
}

trie::~trie()
{
	for (trie_node *& node : list)
	{
		delete node;
	}
	list.clear();
}

size_t trie::size() const
{
    return m_size;
}

bool trie::empty() const
{
    return m_size == 0;
}

bool trie::erase(const string& str)
{
	if (findInChildren(m_root, str) && deleteWordFromChildren(m_root, str))
	{
		m_size--;
		return true;
	}

	return false;;
}

bool trie::insert(const string& str)
{
	if (str.empty() && (m_root->payload != ' '))
	{
		m_root->payload = ' ';
		m_size++;
		return true;
	}

	if (insertAsChild(m_root, str))
	{
		m_size++;
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

vector<string> trie::search_by_prefix(const string& prefix) const
{
	vector<string> words = {};
	string phrase = "";

	int i = 0;
	bool found = false;
	trie_node * foo = m_root;

	while (prefix[i] != '\0')
	{
		for (int j = 0; j < num_chars; j++)
		{
			if (foo->children[j] != nullptr && foo->children[j]->payload == prefix[i])
			{
				foo = foo->children[j];
				phrase.push_back(prefix[i]);
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

vector<string> trie::get_prefixes(const string& str) const
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


// 
// FUNKCE A PROMÌNNÉ PODLE "TRIE.HPP" - WORD CURSOR


word_cursor::word_cursor(const trie_node* ptr)
{
	if (ptr->payload == ' ')
	{
		m_ptr = ptr;
	}
	else
	{
		for (int i = 0; i < num_chars; i++)
		{
			if (ptr->children[i] != nullptr)
			{
				if (ptr->children[i]->is_terminal)
				{
					m_ptr = ptr->children[i];
					break;
				}

				ptr = ptr->children[i];
				i = -1;
			}
		}
	}
}

word_cursor trie::get_word_cursor() const
{
	return word_cursor(m_root);
}

bool word_cursor::has_word() const
{
	return m_ptr != nullptr;
}

string word_cursor::read_word() const
{
	if (m_ptr->payload == ' ')
	{
		return "";
	}

	return getThisWord(const_cast<trie_node*>(m_ptr), "");
}

void word_cursor::move_to_next_word() 
{
	m_ptr = addNextWord(m_ptr);
}
