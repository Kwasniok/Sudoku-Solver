//
//  Cell.h
//  Sudoku Solver
//
//  Created by Jens Kwasniok on 20.06.16.
//  Copyright © 2016 Jens Kwasniok. All rights reserved.
//

#ifndef Cell_h
#define Cell_h

#include <vector>
#include <algorithm>
#include <iostream>
#include "config.h"
#include "util.h"

#ifdef SUDOKU_SOLVER_CHECK_VAUE_RANGE
#define SUDOKU_SOLVER_VALUE_RANGE_THROW throw (std::out_of_range)
#else
#define SUDOKU_SOLVER_VALUE_RANGE_THROW noexcept
#endif

namespace sudoku_solver {

	using value_t = unsigned int;

	class Single_Value_Cell {
		
	public:
		Single_Value_Cell() { }
		Single_Value_Cell(value_t v) : _value(v) { }
		Single_Value_Cell(const Single_Value_Cell&) = default;
		Single_Value_Cell(Single_Value_Cell&&) = default;
		Single_Value_Cell& operator=(const Single_Value_Cell&) = default;
		Single_Value_Cell& operator=(Single_Value_Cell&&) = default;
		
		void set_value(value_t v) {_value = v;};
		value_t get_value() const {return _value;}
		
		bool is_empty() const {return _value == 0;}
		void clear() {_value = 0;}
		
		
		static constexpr char get_empty_representation() {return '_';}
		
	private:
		value_t _value = 0;
		
	};
	
	std::ostream& operator<<(std::ostream& os, const Single_Value_Cell& c);
	std::istream& operator>>(std::istream& is, Single_Value_Cell& c);
	

	struct Multiple_Value_Cell {
		
	public:
		Multiple_Value_Cell() { }
		Multiple_Value_Cell(std::vector<value_t> values) : _values(values) { }
		Multiple_Value_Cell(value_t value) : _values({value}) { }
		Multiple_Value_Cell(const Multiple_Value_Cell&) = default;
		Multiple_Value_Cell(Multiple_Value_Cell&&) = default;
		Multiple_Value_Cell& operator=(const Multiple_Value_Cell&) = default;
		Multiple_Value_Cell& operator=(Multiple_Value_Cell&&) = default;
		
		void add_value(value_t v);
		void remove_value(const value_t v) noexcept;
		
		void set_values(std::vector<value_t> vs);
		std::vector<value_t>& get_values() {return _values;}
		const std::vector<value_t>& get_values() const {return _values;}
		
		bool is_empty() const {return _values.empty();}
		void make_final() {_final = true;}
		bool is_final() const {return _final;}
		void clear() {_values.clear();}
		
	private:
		std::vector<value_t> _values;
		bool _final = false;
	};
	
	std::ostream& operator<<(std::ostream& os, const Multiple_Value_Cell& c);
	std::istream& operator>>(std::istream& is, Multiple_Value_Cell& c);
	

	std::ostream& operator<<(std::ostream& os, const Single_Value_Cell& c) {
		if (c.is_empty()) os << Single_Value_Cell::get_empty_representation();
		else os << c.get_value();
		return os;
	}

	std::istream& operator>>(std::istream& is, Single_Value_Cell& cell) {
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

#ifdef SUDOKU_SOLVER_CELL_PRINT_DETAILED
	std::ostream& operator<<(std::ostream& os, const Multiple_Value_Cell& c) {
		os << '[';
		for (value_t v = 1 ; v <= max_value; ++v) {
			if (has_value(c.get_values(), v)) os << v;
			else os << ' ';
		}
		os << ']';
		return os;
	}
#else
	std::ostream& operator<<(std::ostream& os, const Multiple_Value_Cell& c) {
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
#endif

	std::istream& operator>>(std::istream& is, Multiple_Value_Cell& cell) {
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
}

#endif /* Cell_h */
