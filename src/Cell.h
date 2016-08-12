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
#include <iostream>

namespace sudoku_solver {

	//! numerical datatype represnting a cell value
	using value_t = unsigned int;

	//! cell type for cells with one or no value
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
		value_t max_value() const {return _value;}
		
		bool is_empty() const {return _value == 0;}
		bool is_final() const {return _value != 0;}
		void clear() {_value = 0;}
		
		//! returns the value used to mark empty cells in string representations
		static constexpr char get_empty_representation() {return '_';}
		
	private:
		value_t _value = 0;
		
	};
	
	std::ostream& operator<<(std::ostream& os, const Single_Value_Cell& c);
	std::istream& operator>>(std::istream& is, Single_Value_Cell& c);
	
	
	//! cell type for cells supporting multiple values per cell (or none)
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
		value_t max_value() const;
		
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
	
}

#endif /* Cell_h */
