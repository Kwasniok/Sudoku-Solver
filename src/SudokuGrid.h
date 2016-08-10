//
//  SudokuGrid.hpp
//  Sudoku Solver
//
//  Created by Jens Kwasniok on 20.06.16.
//  Copyright © 2016 Jens Kwasniok. All rights reserved.
//

#ifndef SudokuGrid_hpp
#define SudokuGrid_hpp

#include <vector>
#include <iostream>
#include "Cell.h"

namespace sudoku_solver {
	
	template <class cell_t>
	class Sudoku_Grid;
	
	//! template for single or multiple valued cell grids
	template <class cell_t>
	class Sudoku_Grid {
		
	public:
		
		using grid_t = std::vector<std::vector<cell_t>>;
		using index_t = typename grid_t::size_type ;
		
		Sudoku_Grid(index_t size=9);
		Sudoku_Grid(const Sudoku_Grid<cell_t>&);
		Sudoku_Grid(Sudoku_Grid<cell_t>&&);
		Sudoku_Grid& operator=(const Sudoku_Grid<cell_t>&);
		Sudoku_Grid& operator=(Sudoku_Grid<cell_t>&&);
		
		void set_cell(const index_t x, const index_t y, const cell_t& c) throw (std::out_of_range);
		void set_cell(const index_t x, const index_t y, cell_t&& c) throw (std::out_of_range);
		cell_t& get_cell(const index_t x, const index_t y) throw (std::out_of_range);
		const cell_t& get_cell(const index_t x, const index_t y) const throw (std::out_of_range);
		
		index_t size() const {return _size;}
		
	private:
		grid_t _grid;
		index_t _size;
	};
	
	
	using Single_Value_Sudoku_Grid = Sudoku_Grid<Single_Value_Cell>;
	using  Multiple_Value_Sudoku_Grid = Sudoku_Grid<Multiple_Value_Cell>;
	
	template<class cell_t>
	std::ostream& operator<<(std::ostream& os, const Sudoku_Grid<cell_t>& sg);
	template<class cell_t>
	std::istream& operator>>(std::istream& is, Sudoku_Grid<cell_t>& sg);
	void print_grid(std::ostream& os, const Multiple_Value_Sudoku_Grid& g, const value_t max_value);
	
	Multiple_Value_Sudoku_Grid to_multiple_value_cell_grid(const Single_Value_Sudoku_Grid& rhs);

	template <class cell_t>
	Sudoku_Grid<cell_t>::Sudoku_Grid(index_t size) {
		for (size_t x=0;  x < size; ++x)
			_grid.push_back(std::vector<cell_t>(size));
		
		_size = size;
	}
	
	template <class cell_t>
	Sudoku_Grid<cell_t>::Sudoku_Grid(const Sudoku_Grid<cell_t>& rhs)
	: _grid(rhs._grid), _size(rhs._size)
	{ }
	
	template <class cell_t>
	Sudoku_Grid<cell_t>::Sudoku_Grid(Sudoku_Grid<cell_t>&& rhs)
	: _grid(std::move(rhs._grid)), _size(rhs._size)
	{
		rhs._size = 0;
	}
	
	template <class cell_t>
	Sudoku_Grid<cell_t>& Sudoku_Grid<cell_t>::operator=(const Sudoku_Grid<cell_t>& rhs) {
		_grid = rhs._grid;
		_size = rhs._size;
		
		return *this;
	}
	
	template <class cell_t>
	Sudoku_Grid<cell_t>& Sudoku_Grid<cell_t>::operator=(Sudoku_Grid<cell_t>&& rhs)
	{
		_grid = std::move(rhs._grid);
		_size = rhs._size;
		rhs._size = 0;
		
		return *this;
	}
	
	template <class cell_t>
	void Sudoku_Grid<cell_t>::set_cell(const index_t x, const index_t y, const cell_t& c) throw(std::out_of_range) {
		_grid.at(x).at(y) = c;
	}
	
	template <class cell_t>
	void Sudoku_Grid<cell_t>::set_cell(const index_t x, const index_t y, cell_t&& c) throw(std::out_of_range) {
		_grid.at(x).at(y) = std::move(c);
	}
	
	template <class cell_t>
	cell_t& Sudoku_Grid<cell_t>::get_cell(const index_t x, const index_t y) throw (std::out_of_range) {
		return _grid.at(x).at(y);
	}
	
	template <class cell_t>
	const cell_t& Sudoku_Grid<cell_t>::get_cell(const index_t x, const index_t y) const throw (std::out_of_range) {
		return _grid.at(x).at(y);
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
		Sudoku_Grid<cell_t> tmp {sg.size()};
		cell_t cell;
		
		for (int x = 0; x < tmp.size(); ++x) {
			for (int y = 0; y < tmp.size(); ++y) {
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
