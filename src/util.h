//
//  util.h
//  Sudoku Solver
//
//  Created by Jens Kwasniok on 21.06.16.
//  Copyright © 2016 Jens Kwasniok. All rights reserved.
//

#ifndef util_h
#define util_h

#include <algorithm>
#include <utility>
#include <vector>

//! @param c a container
//! @param v value to search for
//! @return true if value is in container
template<class T, class V>
inline bool has_value(T& c, const V& v) {
	return std::find(c.begin(), c.end(), v) != c.end();
}

//! Removes (first occurence of) a value from the std::vector and returns true if value was in vector.
//! his might change the order of the std::vector.
//! @param c std::vector of any type
//! @param v value to be removed from std::vector
//! @return true if values was in vecotor
template<class V>
inline bool removed(std::vector<V>& c, const V& v) {
	// search for first appearence of v in c
	for (auto it = c.begin(); it != c.end(); ++it) {
		if (v == *it) {
			// swap value to back position and remove it
			iter_swap(it, c.rbegin());
			c.pop_back();
			return true;;
		}
	}
	return false;
}

//! Removes (first occurence of) a value from the std::vector.
//! his might change the order of the std::vector.
//! @param c std::vector of any type
//! @param v value to be removed from std::vector
template<class V>
inline void remove(std::vector<V>& c, const V& v) {
	removed(c, v);
}

#endif /* util_h */
