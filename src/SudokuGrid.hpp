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
		
		void set_cell(const index_t x, const index_t y, const cell_t& c) throw (std::out_of_range);
		cell_t& get_cell(const index_t x, const index_t y) throw (std::out_of_range);
		const cell_t& get_cell(const index_t x, const index_t y) const throw (std::out_of_range);
		
		static size_t size() {return cell_t::get_max_value();}
		
	private:
		grid_t _grid;
	};
	
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
	
}

#endif /* SudokuGrid_hpp */
