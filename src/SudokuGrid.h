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
	
	template <
		class cell_t,
		typename Sudoku_Grid<cell_t>::index_t box_size_x,
		typename Sudoku_Grid<cell_t>::index_t box_size_y
	>
	int std_box_index_func(typename Sudoku_Grid<cell_t>::index_t x, typename Sudoku_Grid<cell_t>::index_t y);
	
	//! template for single or multiple valued cell grids
	template <class cell_t>
	class Sudoku_Grid {
		
	public:
		
		using grid_t = std::vector<std::vector<cell_t>>;
		using index_t = typename grid_t::size_type ;
		//! tpye of a function which maps each cell cooridinate to the index of its box
		using get_box_index_t = int (index_t, index_t);
		
		Sudoku_Grid(index_t size_x=9, index_t size_y=9, int (&box_index_func) (index_t x, index_t y) = std_box_index_func<cell_t, 3, 3>);
		Sudoku_Grid(const Sudoku_Grid<cell_t>&);
		Sudoku_Grid(Sudoku_Grid<cell_t>&&);
		Sudoku_Grid& operator=(const Sudoku_Grid<cell_t>&);
		Sudoku_Grid& operator=(Sudoku_Grid<cell_t>&&);
		
		void set_cell(const index_t x, const index_t y, const cell_t& c) throw (std::out_of_range);
		void set_cell(const index_t x, const index_t y, cell_t&& c) throw (std::out_of_range);
		cell_t& get_cell(const index_t x, const index_t y) throw (std::out_of_range);
		const cell_t& get_cell(const index_t x, const index_t y) const throw (std::out_of_range);
		
		index_t size_x() const {return _size_x;}
		index_t size_y() const {return _size_y;}
		
		//! @return a function which maps each cell cooridinate to the index of its box
		get_box_index_t& get_box_index_func() const {return *_get_box_index;}
		
	private:
		grid_t _grid;
		index_t _size_x;
		index_t _size_y;
		get_box_index_t* _get_box_index;
	};
	
	
	using Single_Value_Sudoku_Grid = Sudoku_Grid<Single_Value_Cell>;
	using  Multiple_Value_Sudoku_Grid = Sudoku_Grid<Multiple_Value_Cell>;
	
	template<class cell_t>
	std::ostream& operator<<(std::ostream& os, const Sudoku_Grid<cell_t>& sg);
	template<class cell_t>
	std::istream& operator>>(std::istream& is, Sudoku_Grid<cell_t>& sg);
	
	Multiple_Value_Sudoku_Grid to_multiple_value_cell_grid(const Single_Value_Sudoku_Grid& rhs);

	template <class cell_t>
	Sudoku_Grid<cell_t>::Sudoku_Grid(index_t size_x, index_t size_y, int (&box_index_func) (index_t x, index_t y))
	: _get_box_index(box_index_func) {
		for (size_t x=0;  x < size_x; ++x)
			_grid.push_back(std::vector<cell_t>(size_y));
		
		_size_x = size_x;
		_size_y = size_y;
	}
	
	template <class cell_t>
	Sudoku_Grid<cell_t>::Sudoku_Grid(const Sudoku_Grid<cell_t>& rhs)
	: _grid(rhs._grid), _size_x(rhs._size_x), _size_y(rhs._size_y), _get_box_index(rhs._get_box_index)
	{ }
	
	template <class cell_t>
	Sudoku_Grid<cell_t>::Sudoku_Grid(Sudoku_Grid<cell_t>&& rhs)
	: _grid(std::move(rhs._grid)), _size_x(rhs._size_x), _size_y(rhs._size_y), _get_box_index(rhs._get_box_index)
	{
		rhs._size_x = 0;
		rhs._size_y = 0;
		rhs._get_box_index = nullptr;
	}
	
	template <class cell_t>
	Sudoku_Grid<cell_t>& Sudoku_Grid<cell_t>::operator=(const Sudoku_Grid<cell_t>& rhs) {
		_grid = rhs._grid;
		_size_x = rhs._size_x;
		_size_y = rhs._size_y;
		
		return *this;
	}
	
	template <class cell_t>
	Sudoku_Grid<cell_t>& Sudoku_Grid<cell_t>::operator=(Sudoku_Grid<cell_t>&& rhs)
	{
		_grid = std::move(rhs._grid);
		_size_x = rhs._size_x;
		_size_y = rhs._size_y;
		rhs._size_x = 0;
		rhs._size_y = 0;
		rhs._get_box_index = nullptr;
		
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
		for (int x = 0; x < sg.size_x(); ++x) {
			if (first_x) first_x = false;
			else os << std::endl;
			
			bool first_y = true;
			for (int y = 0; y < sg.size_y(); ++y) {
				if(first_y) first_y = false;
				else os << ' ';
				
				os << sg.get_cell(x,y);
			}
		}
		return os;
	}
	
	template<class cell_t>
	std::istream& operator>>(std::istream& is, Sudoku_Grid<cell_t>& sg) {
		Sudoku_Grid<cell_t> tmp {sg.size_x(), sg.size_y(), sg.get_box_index_func()};
		cell_t cell;
		
		for (int x = 0; x < tmp.size_x(); ++x) {
			for (int y = 0; y < tmp.size_y(); ++y) {
				is >> cell;
				tmp.set_cell(x,y, cell);
			}
		}
		
		if (is.good())
			sg = std::move(tmp);
		return is;
	}
	
	template <
		class cell_t,
		typename Sudoku_Grid<cell_t>::index_t box_size_x,
		typename Sudoku_Grid<cell_t>::index_t box_size_y
	>
	int std_box_index_func(typename Sudoku_Grid<cell_t>::index_t x, typename Sudoku_Grid<cell_t>::index_t y) {
		return int((x/box_size_x) * box_size_y + (y/box_size_y)); // rounding intended!
	}

}

#endif /* SudokuGrid_hpp */
