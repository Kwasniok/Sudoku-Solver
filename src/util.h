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

template<class T, class V>
inline bool has_value(T& c, const V& v) {
	return std::find(c.begin(), c.end(), v) != c.end();
}

template<class V>
inline bool removed(std::vector<V>& c, const V& v) {
	for (auto it = c.begin(); it != c.end(); ++it) {
		if (v == *it) {
			iter_swap(it, c.rbegin());
			c.pop_back();
			return true;;
		}
	}
	return false;
}

#endif /* util_h */
