//
//  Cell.cpp
//  Sudoku Solver
//
//  Created by Jens Kwasniok on 20.07.16.
//  Copyright © 2016 Jens Kwasniok. All rights reserved.
//

#include "Cell.h"
#include <algorithm>

using namespace sudoku_solver;

std::ostream& sudoku_solver::operator<<(std::ostream& os, const Single_Value_Cell& c) {
	if (c.is_empty()) os << Single_Value_Cell::get_empty_representation();
	else os << c.get_value();
	return os;
}

std::istream& sudoku_solver::operator>>(std::istream& is, Single_Value_Cell& cell) {
	Single_Value_Cell tmp;
	value_t v;
	char c;
	is >> c;
	if (c != Single_Value_Cell::get_empty_representation()) {
		is.putback(c);
		is >> v;
		tmp.set_value(v);
	} else {
		tmp.clear();
	}
	cell = std::move(tmp);
	return is;
}


void Multiple_Value_Cell::add_value(value_t v) {
	if (std::find(_values.begin(), _values.end(), v) == _values.end()) // has not
		_values.push_back(v);
}

void Multiple_Value_Cell::remove_value(const value_t v) noexcept {
	for (auto it = _values.begin(); it != _values.end(); ++it) {
		if (*it == v) {
			std::iter_swap(it, _values.rbegin());
			_values.pop_back();
		}
	}
}

void Multiple_Value_Cell::set_values(std::vector<value_t> vs) {
	_values = vs;
}

value_t Multiple_Value_Cell::max_value() const {
	return *std::max(_values.begin(), _values.end());
}

std::ostream& sudoku_solver::operator<<(std::ostream& os, const Multiple_Value_Cell& c) {
	os << '[';
	bool first = true;
	for (auto it = c.get_values().begin(); it != c.get_values().end(); ++it) {
		if (first)
			first = false;
		else
			os << ' ';
		os << *it;
	}
	os << ']';
	return os;
}

std::istream& sudoku_solver::operator>>(std::istream& is, Multiple_Value_Cell& cell) {
	Multiple_Value_Cell tmp;
	value_t v;
	char c;
	is >> c;
	if (is.fail() || c != '[') {is.setstate(std::ios_base::failbit | is.rdstate()); return is;}
	for (;/*ever*/;) {
		is >> c;
		if (is.fail()) return is;
		if (c == ']') break;
		else {
			is.putback(c);
			is >> v;
			if (is.fail()) return is;
			tmp.add_value(v);
		}
	}
	cell = std::move(tmp);
	return is;
}
