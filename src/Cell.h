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

	template <value_t max_value>
	class Single_Value_Cell {
		
	public:
		Single_Value_Cell() { }
		Single_Value_Cell(value_t v) : _value(v) { }
		Single_Value_Cell(const Single_Value_Cell<max_value>&) = default;
		Single_Value_Cell(Single_Value_Cell<max_value>&&) = default;
		Single_Value_Cell& operator=(const Single_Value_Cell<max_value>&) = default;
		Single_Value_Cell& operator=(Single_Value_Cell<max_value>&&) = default;
		
		void set_value(value_t v) SUDOKU_SOLVER_VALUE_RANGE_THROW;
		value_t get_value() const {return _value;}
		
		bool is_empty() const {return _value == 0;}
		void clear() {_value = 0;}
		
		
		static constexpr value_t get_max_value() {return max_value;}
		static constexpr char get_empty_representation() {return '_';}
		
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
		Multiple_Value_Cell(value_t value) : _values({value}) { }
		Multiple_Value_Cell(const Multiple_Value_Cell<max_value>&) = default;
		Multiple_Value_Cell(Multiple_Value_Cell<max_value>&&) = default;
		Multiple_Value_Cell& operator=(const Multiple_Value_Cell<max_value>&) = default;
		Multiple_Value_Cell& operator=(Multiple_Value_Cell<max_value>&&) = default;
		
		void add_value(value_t v) SUDOKU_SOLVER_VALUE_RANGE_THROW;
		void remove_value(const value_t v) noexcept;
		
		void set_values(std::vector<value_t> vs) SUDOKU_SOLVER_VALUE_RANGE_THROW;
		std::vector<value_t>& get_values() {return _values;}
		const std::vector<value_t>& get_values() const {return _values;}
		
		bool is_empty() const {return _values.empty();}
		void make_final() {_final = true;}
		bool is_final() const {return _final;}
		void clear() {_values.clear();}
		
		
		static const value_t get_max_value() {return max_value;}
		
	private:
		std::vector<value_t> _values;
		bool _final = false;
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
		if (c.is_empty()) os << Single_Value_Cell<max_value>::get_empty_representation();
		else os << c.get_value();
		return os;
	}

	template<value_t max_value>
	std::istream& operator>>(std::istream& is, Single_Value_Cell<max_value>& cell) {
		Single_Value_Cell<max_value> tmp;
		value_t v;
		char c;
		is >> c;
		if (c != Single_Value_Cell<max_value>::get_empty_representation()) {
			is.putback(c);
			is >> v;
			tmp.set_value(v);
		} else {
			tmp.clear();
		}
		cell = std::move(tmp);
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

#ifdef SUDOKU_SOLVER_CELL_PRINT_DETAILED
	template<value_t max_value>
	std::ostream& operator<<(std::ostream& os, const Multiple_Value_Cell<max_value>& c) {
		os << '[';
		for (value_t v = 1 ; v <= max_value; ++v) {
			if (has_value(c.get_values(), v)) os << v;
			else os << ' ';
		}
		os << ']';
		return os;
	}
#else
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
#endif

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
