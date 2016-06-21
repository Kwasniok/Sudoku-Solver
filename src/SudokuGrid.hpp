//
//  SudokuGrid.hpp
//  Sudoku Solver
//
//  Created by Jens Kwasniok on 20.06.16.
//  Copyright © 2016 Jens Kwasniok. All rights reserved.
//

#ifndef SudokuGrid_hpp
#define SudokuGrid_hpp

#include <limits>
#include <vector>
#include <iostream>
#include "config.h"
#include "Cell.h"

namespace sudoku_solver {
	
	template <class cell_t>
	class Sudoku_Grid {
		
	public:
		
		typedef std::vector<std::vector<cell_t>> grid_t;
		typedef typename grid_t::size_type index_t;
		typedef index_t size_t;
		
		Sudoku_Grid();
		Sudoku_Grid(const Sudoku_Grid<cell_t>&) = default;
		Sudoku_Grid(Sudoku_Grid<cell_t>&&) = default;
		Sudoku_Grid& operator=(const Sudoku_Grid<cell_t>&) = default;
		Sudoku_Grid& operator=(Sudoku_Grid<cell_t>&&) = default;
		
		void set_cell(const index_t x, const index_t y, const cell_t& c) throw (std::out_of_range);
		cell_t& get_cell(const index_t x, const index_t y) throw (std::out_of_range);
		const cell_t& get_cell(const index_t x, const index_t y) const throw (std::out_of_range);
		
		static size_t size() {return cell_t::get_max_value();}
		
	private:
		grid_t _grid;
	};
	
	template<value_t max_value>
	Sudoku_Grid<Multiple_Value_Cell<max_value>> to_multiple_value_cell_grid(const Sudoku_Grid<Single_Value_Cell<max_value>>& rhs);

	template<class cell_t>
	std::ostream& operator<<(std::ostream& os, const Sudoku_Grid<cell_t>& sg);
	template<class cell_t>
	std::istream& operator>>(std::istream& is, Sudoku_Grid<cell_t>& sg);
	
	template <class cell_t>
	Sudoku_Grid<cell_t>::Sudoku_Grid() {
		for (size_t x=0;  x<cell_t::get_max_value(); ++x)
			_grid.push_back(std::vector<cell_t>(cell_t::get_max_value()));
		
		if (_grid.size() != cell_t::get_max_value())
			_grid.clear();
	}
	
	template <class cell_t>
	void Sudoku_Grid<cell_t>::set_cell(const index_t x, const index_t y, const cell_t& c) throw(std::out_of_range) {
		_grid.at(x).at(y) = c;
	}
	
	template <class cell_t>
	cell_t& Sudoku_Grid<cell_t>::get_cell(const index_t x, const index_t y) throw (std::out_of_range) {
		return _grid.at(x).at(y);
	}
	
	template <class cell_t>
	const cell_t& Sudoku_Grid<cell_t>::get_cell(const index_t x, const index_t y) const throw (std::out_of_range) {
		return _grid.at(x).at(y);
	}
	
	template<value_t max_value>
	Sudoku_Grid<Multiple_Value_Cell<max_value>> to_multiple_value_cell_grid(const Sudoku_Grid<Single_Value_Cell<max_value>>& rhs) {
		Sudoku_Grid<Multiple_Value_Cell<max_value>> tmp;
		for (int x = 0; x < rhs.size(); ++x) {
			for (int y = 0; y < rhs.size(); ++y) {
				tmp.set_cell(x, y, {{rhs.get_cell(x,y).get_value()}});
			}
		}
		return tmp;
	}
	
	template<class cell_t>
	std::ostream& operator<<(std::ostream& os, const Sudoku_Grid<cell_t>& sg) {
		bool first_x = true;
		for (int x = 0; x < sg.size(); ++x) {
			if (first_x) first_x = false;
			else os << std::endl;
			
			bool first_y = true;
			for (int y = 0; y < sg.size(); ++y) {
				if(first_y) first_y = false;
				else os << ' ';
				
				os << sg.get_cell(x,y);
			}
		}
		return os;
	}
	
	template<class cell_t>
	std::istream& operator>>(std::istream& is, Sudoku_Grid<cell_t>& sg) {
		Sudoku_Grid<cell_t> tmp;
		cell_t cell;
		
		for (int x = 0; x < sg.size(); ++x) {
			for (int y = 0; y < sg.size(); ++y) {
				is >> cell;
				tmp.set_cell(x,y, cell);
			}
		}
		
		if (is.good())
			sg = std::move(tmp);
		return is;
	}
	
}

#endif /* SudokuGrid_hpp */
