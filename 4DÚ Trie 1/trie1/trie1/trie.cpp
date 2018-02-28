#include "trie.hpp"

using namespace std;


// 
// VLASTNÍ FUNKCE A PROMÌNNÉ


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
// FUNKCE A PROMÌNNÉ DLE "TRIE.HPP"


void init(trie& trie)
{
	trie.root = new trie_node;
	trie.root->is_terminal = true;
	list.push_back(trie.root);
}

bool insert(trie& trie, const string& str)
{
	if (str.empty() && (trie.root->payload != ' '))
	{
		trie.root->payload = ' ';
	}

	if (insertAsChild(trie.root, str))
	{
		trie.size++;
		return true;
	}

	return false;
}

bool erase(trie& trie, const string& str)
{
	if (findInChildren(trie.root, str) && deleteWordFromChildren(trie.root, str))
	{
		trie.size--;
		return true;
	}

	return false;
}

void deallocate(trie& trie)
{
	for (trie_node *& node : list)
	{
		delete node;
	}
	list.clear();
}

bool contains(const trie& trie, const string& str)
{
	if (str.empty() && (trie.root->payload == ' '))
	{
		return true;
	}
	return findInChildren(trie.root, str);
}

void insert_all(trie& trie, const vector<string>& items)
{
	for (string word : items)
	{
		if (word.empty() && (trie.root->payload != ' '))
		{
			trie.root->payload = ' ';
			trie.size++;
		}

		if (insertAsChild(trie.root, word))
		{
			trie.size++;
		}
	}
}

size_t size(const trie& trie)
{
	return trie.size;
}

bool empty(const trie& trie)
{
	return trie.size == 0;
}
