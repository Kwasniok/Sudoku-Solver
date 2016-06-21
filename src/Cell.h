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

#ifdef SUDOKU_SOLVER_CHECK_VAUE_RANGE
#define SUDOKU_SOLVER_VALUE_RANGE_THROW throw (std::out_of_range)
#else
#define SUDOKU_SOLVER_VALUE_RANGE_THROW noexcept
#endif

namespace sudoku_solver {

	typedef unsigned int value_t;

	template <value_t max_value>
	class Single_Value_Cell {
		
	public:
		Single_Value_Cell() { }
		Single_Value_Cell(value_t v) : _value(v) { }
		
		void set_value(value_t v) SUDOKU_SOLVER_VALUE_RANGE_THROW;
		value_t get_value() const {return _value;}
		
		void clear() {_value = 0;}
		
		
		static const value_t get_max_value() {return max_value;}
		
	private:
		value_t _value = 0;
		
	};
	
	template<value_t max_value>
	std::ostream& operator<<(std::ostream& os, const Single_Value_Cell<max_value>& c);
	template<value_t max_value>
	std::istream& operator>>(std::istream& is, Single_Value_Cell<max_value>& c);
	

	template <value_t max_value>
	struct Multiple_Value_Cell {
		
	public:
		Multiple_Value_Cell() { }
		Multiple_Value_Cell(std::vector<value_t> values) : _values(values) { }
		
		void add_value(value_t v) SUDOKU_SOLVER_VALUE_RANGE_THROW;
		void remove_value(const value_t v) noexcept;
		
		void set_values(std::vector<value_t> vs) SUDOKU_SOLVER_VALUE_RANGE_THROW;
		std::vector<value_t>& get_values() {return _values;}
		const std::vector<value_t>& get_values() const {return _values;}
		
		void clear() {_values.clear();}
		
		
		static const value_t get_max_value() {return max_value;}
		
	private:
		std::vector<value_t> _values;
	};
	
	template<value_t max_value>
	std::ostream& operator<<(std::ostream& os, const Multiple_Value_Cell<max_value>& c);
	template<value_t max_value>
	std::istream& operator>>(std::istream& is, Multiple_Value_Cell<max_value>& c);
	
	
	template<value_t max_value>
	void Single_Value_Cell<max_value>::set_value(value_t v) SUDOKU_SOLVER_VALUE_RANGE_THROW {
#ifdef SUDOKU_SOLVER_CHECK_VAUE_RANGE
		if (v > max_value) throw std::out_of_range("value higher than max in Single_Value_Cell::set_value");
		if (v == 0) throw std::out_of_range("value equals 0 in Single_Value_Cell::set_value");
#endif
		_value = v;
	}

	template<value_t max_value>
	std::ostream& operator<<(std::ostream& os, const Single_Value_Cell<max_value>& c) {
		return (os << c.get_value());
	}

	template<value_t max_value>
	std::istream& operator>>(std::istream& is, Single_Value_Cell<max_value>& c) {
		Single_Value_Cell<max_value> tmp;
		value_t v;
		is >> v;
		tmp.set_value(v);
		c = std::move(tmp);
		return is;
	}


	template<value_t max_value>
	void Multiple_Value_Cell<max_value>::add_value(value_t v) SUDOKU_SOLVER_VALUE_RANGE_THROW {
#ifdef SUDOKU_SOLVER_CHECK_VAUE_RANGE
		if (v > max_value) throw std::out_of_range("value higher than max in Multiple_Value_Cell::add_value");
		if (v == 0) throw std::out_of_range("value equals 0 in Multiple_Value_Cell::add_value");
#endif
		if (std::find(_values.begin(), _values.end(), v) == _values.end()) // has not
			_values.push_back(v);
	}
	
	template<value_t max_value>
	void Multiple_Value_Cell<max_value>::remove_value(const value_t v) noexcept {
		for (auto it = _values.begin(); it != _values.end(); ++it) {
			if (*it == v) {
				std::iter_swap(it, _values.rbegin());
				_values.pop_back();
			}
		}
	}
	
	template<value_t max_value>
	void Multiple_Value_Cell<max_value>::set_values(std::vector<value_t> vs) SUDOKU_SOLVER_VALUE_RANGE_THROW {
#ifdef SUDOKU_SOLVER_CHECK_VAUE_RANGE
		for (auto it = vs.begin(); it != vs.end(); ++it) {
			if (*it > max_value) throw std::out_of_range("value higher than max in Multiple_Value_Cell::add_value");
			if (*it == 0) throw std::out_of_range("value equals 0 in Multiple_Value_Cell::set_values");
		}
#endif
		_values = vs;
	}

	template<value_t max_value>
	std::ostream& operator<<(std::ostream& os, const Multiple_Value_Cell<max_value>& c) {
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

	template<value_t max_value>
	std::istream& operator>>(std::istream& is, Multiple_Value_Cell<max_value>& cell) {
		Multiple_Value_Cell<max_value> tmp;
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
