#pragma once

#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;
/**
* Note that when the value of an element is changed so that the comparator orders it differently, the behavior is undefined.
*/

template <typename T, typename Comparator = std::less<T>>
class flat_set {
private:
	vector<T> set;
	Comparator comp;

public:
	// These types need to be accesible from the outside:
	// iterator
	// const_iterator
	// size_type
	// value_type
	
	typedef typename vector<T>::const_iterator const_iterator;
	typedef typename vector<T>::iterator iterator;
	typedef typename vector<T>::size_type size_type;
	typedef typename vector<T>::value_type value_type;

	// Special member functions
	flat_set()
	{
		set = {};
	}

	flat_set(Comparator const& cmp)
	{
		set = {};
		comp = cmp;
	}

	flat_set(flat_set const& rhs)
		:flat_set()
	{
		for (int i = 0; i < rhs.size(); i++)
		{
			set.push_back(rhs.set[i]);
		}
	}

	flat_set(flat_set && rhs)
	{
		for (int i = 0; i < rhs.size(); i++)
		{
			set.push_back(move(rhs.set[i]));
		}

		std::sort(set.begin(), set.end(), comp);
	}

	flat_set& operator=(flat_set const& rhs)
	{
		flat_set(rhs).swap(*this);
		return *this;
	}
	
	flat_set& operator=(flat_set && rhs)
	{
		flat_set(std::move(rhs)).swap(*this);
		return *this;
	}

	~flat_set() {}


	// Constructs flat_set from elements in range [first, last)
	template <typename InputIterator>
	flat_set(InputIterator first, InputIterator last)
	{
		set = vector<T>(first, last);
		std::sort(set.begin(), set.end(), comp);
		
		set.erase(std::unique(
			set.begin(),
			set.end(),
			[this](T const &a, T const &b) {
				return !comp(b, a) && !comp(a, b);
			}),
			set.end());
	}

	template <typename InputIterator>
	flat_set(InputIterator first, InputIterator last, Comparator const& cmp)
	{
		comp = cmp;
		insert(first, last, comp);
	}


	// Insert overloads
	pair<iterator, bool> insert(T const& v)
	{
		using std::begin;
		using std::end;
		auto i = std::lower_bound(set.begin(), set.end(), v, comp);
		if (i != set.end() && !comp(v, *i))
		{
			return std::make_pair(i, false);
		}
		else
		{
			auto foo = this->set.insert(i, v);
			return std::make_pair(foo, true);
		}
	}

	pair<iterator, bool> insert(T&& v)
	{
		using std::begin;
		using std::end;
		auto i = std::lower_bound(set.begin(), set.end(), v, comp);
		if ((i != set.end()) && !comp(v, *i))
		{
			return std::make_pair(i, false);
		}
		else
		{
			auto foo = this->set.insert(i, std::forward<T>(v));
			return std::make_pair(foo, true);
		}
	}

	// Inserts [first, last) range of elements
	template <typename InputIterator>
	void insert(InputIterator first, InputIterator last)
	{
		set = vector<T>(first, last);
		std::sort(set.begin(), set.end(), comp);
		set.erase(std::unique(
			set.begin(),
			set.end(),
			[this](T const &a, T const &b) {
				return !comp(b, a) && !comp(a, b);
			}),
			set.end());
	}

	// Erase overloads
	// Deletes element pointed-to by i, returns iterator to the next element
	iterator erase(const_iterator i)
	{
		return set.erase(i);
	}

	// Deletes elements in range [first, last), returns iterator to the next element
	iterator erase(const_iterator first, const_iterator last)
	{
		return set.erase(first, last);
	}

	// Deletes element equal to key if it is present, returns how many elements were deleted
	size_type erase(value_type const& key)
	{
		size_type i = 0;
		auto foo = std::equal_range(set.begin(), set.end(), key);
		iterator first = foo.first;
		iterator second = foo.second;

		while (first != second)
		{
			erase(first++);
			i++;
		}

		return i;
	}

	// Iterator member functions
	iterator begin() noexcept
	{
		return set.begin();
	}

	iterator end() noexcept
	{
		return set.end();
	}

	const_iterator begin() const noexcept
	{
		return set.begin();
	}

	const_iterator end() const noexcept
	{
		return set.end();
	}

	const_iterator cbegin() const noexcept
	{
		return set.begin();
	}

	const_iterator cend() const noexcept
	{
		return set.end();
	}

	// The usual queries
	bool empty() const
	{
		return set.empty();
	}

	size_type size() const
	{
		return set.size();
	}

	size_type capacity() const
	{
		return set.capacity();
	}

	void reserve(size_type c)
	{
		set.reserve(c);
	}

	void clear()
	{
		set.clear();
	}

	// Lookup member functions
	// Returns iterator to element equivalent to v, or an end iterator if such element is not present
	iterator find(T const& v)
	{
		iterator i = set.begin();
		iterator end = set.end();
		while (i != end) {
			if (equals(*i, v))
			{
				return i;
			}

			i++;
		}

		return i;
	}

	const_iterator find(T const& v) const
	{
		auto foo = [v, this](T const& foo) {
			return (!comp(v, foo) && !comp(foo, v));
		};

		return find_if(set.begin(), set.end(), foo);
	}

	// Returns iterator to first element that is not less than t, end iterator if no such element is present
	iterator lower_bound(T const& t)
	{
		return std::lower_bound(std::begin(set), std::end(set), t, comp);
	}

	const_iterator lower_bound(T const& t) const
	{
		return std::lower_bound(std::begin(set), std::end(set), t, comp);
	}

	// Returns iterator to first element that is greater than t, end iterator if no such element is present
	iterator upper_bound(T const& t)
	{
		return std::upper_bound(std::begin(set), std::end(set), t, comp);
	}

	const_iterator upper_bound(T const& t) const
	{
		return std::upper_bound(std::begin(set), std::end(set), t, comp);
	}

	void swap(flat_set& o)
	{
		std::swap(comp, o.comp);
		std::swap(set, o.set);
	}

	// Lexicographical comparisons
	bool operator==(flat_set const &a) const&
	{
		return this->set == a.set;
	}

	bool operator!=(flat_set const &a) const&
	{
		return this->set != a.set;
	}

	bool operator<(flat_set const &a) const&
	{
		return lexicographical_compare(this->begin(), this->end(), a.begin(), a.end(), comp);
	}

	bool operator<=(flat_set const &a) const&
	{
		return lexicographical_compare(this->begin(), this->end(), a.begin(), a.end(), comp) || set == a.set;
	}

	bool operator>=(flat_set const &a) const&
	{
		return !lexicographical_compare(this->begin(), this->end(), a.begin(), a.end(), comp) || set == a.set;
	}

	bool operator>(flat_set const &a) const&
	{
		return lexicographical_compare(a.begin(), a.end(), this->begin(), this->end(), comp);
	}

	//
	// Moje funkce

	bool equals(T const& a, T const& b)
	{
		return !comp(a, b) && !comp(b, a);
	}

	template<typename InputIterator>
	void insert(InputIterator first, InputIterator last, Comparator cmp)
	{
		set = vector<T>(first, last);
		std::sort(set.begin(), set.end(), cmp);
		set.erase(std::unique(
			set.begin(),
			set.end(),
			[this](T const &a, T const &b) {
				return !comp(b, a) && !comp(a, b);
			}),
			set.end());
	}
};

template <typename T>
void swap(flat_set<T> const& a, flat_set<T> const& b);