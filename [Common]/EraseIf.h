//////////////////////////////////////////
/*
* Copyright (c) 2020-2021 Perchik71 <email:perchik71@outlook.com>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this
* software and associated documentation files (the "Software"), to deal in the Software
* without restriction, including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/
//////////////////////////////////////////

#pragma once

#include <algorithm>
#include <vector>
#include <queue>
#include <map>
#include <list>
#include <forward_list>
#include <set>
#include <unordered_set>
#include <unordered_map>

namespace std {

	// for std::string
	template <class charT, class traits, class A, class Predicate>
	void erase_if(basic_string<charT, traits, A>& c, Predicate pred) {
		c.erase(remove_if(c.begin(), c.end(), pred), c.end());
	}

	// for std::deque
	template <class T, class A, class Predicate>
	void erase_if(deque<T, A>& c, Predicate pred) {
		c.erase(remove_if(c.begin(), c.end(), pred), c.end());
	}

	// for std::vector
	template <class T, class A, class Predicate>
	void erase_if(vector<T, A>& c, Predicate pred) {
		c.erase(remove_if(c.begin(), c.end(), pred), c.end());
	}

	// for std::list
	template <class T, class A, class Predicate>
	void erase_if(list<T, A>& c, Predicate pred) {
		c.remove_if(pred);
	}

	// for std::forward_list
	template <class T, class A, class Predicate>
	void erase_if(forward_list<T, A>& c, Predicate pred) {
		c.remove_if(pred);
	}

	// for std::map
	template <class K, class T, class C, class A, class Predicate>
	void erase_if(map<K, T, C, A>& c, Predicate pred) {
		for (auto i = c.begin(), last = c.end(); i != last; )
			if (pred(*i))
				i = c.erase(i);
			else
				++i;
	}

	// for std::multimap
	template <class K, class T, class C, class A, class Predicate>
	void erase_if(multimap<K, T, C, A> & c, Predicate pred) {
		for (auto i = c.begin(), last = c.end(); i != last; )
			if (pred(*i))
				i = c.erase(i);
			else
				++i;
	}

	// for std::set
	template <class K, class C, class A, class Predicate>
	void erase_if(set<K, C, A> & c, Predicate pred) {
		for (auto i = c.begin(), last = c.end(); i != last; )
			if (pred(*i))
				i = c.erase(i);
			else
				++i;
	}

	// for std::multiset
	template <class K, class C, class A, class Predicate>
	void erase_if(multiset<K, C, A> & c, Predicate pred) {
		for (auto i = c.begin(), last = c.end(); i != last; )
			if (pred(*i))
				i = c.erase(i);
			else
				++i;
	}

	// for std::unordered_map
	template <class K, class T, class H, class P, class A, class Predicate>
	void erase_if(unordered_map<K, T, H, P, A> & c, Predicate pred) {
		for (auto i = c.begin(), last = c.end(); i != last; )
			if (pred(*i))
				i = c.erase(i);
			else
				++i;
	}

	// for std::unordered_multimap
	template <class K, class T, class H, class P, class A, class Predicate>
	void erase_if(unordered_multimap<K, T, H, P, A> & c, Predicate pred) {
		for (auto i = c.begin(), last = c.end(); i != last; )
			if (pred(*i))
				i = c.erase(i);
			else
				++i;
	}

	// for std::unordered_set
	template <class K, class H, class P, class A, class Predicate>
	void erase_if(unordered_set<K, H, P, A> & c, Predicate pred) {
		for (auto i = c.begin(), last = c.end(); i != last; )
			if (pred(*i))
				i = c.erase(i);
			else
				++i;
	}

	// for std::unordered_multiset
	template <class K, class H, class P, class A, class Predicate>
	void erase_if(unordered_multiset<K, H, P, A> & c, Predicate pred) {
		for (auto i = c.begin(), last = c.end(); i != last; )
			if (pred(*i))
				i = c.erase(i);
			else
				++i;
	}

} // namespace std