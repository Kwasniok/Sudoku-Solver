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
#include <math.h>
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
		
		Sudoku_Grid(unsigned int size=9);
		Sudoku_Grid(const Sudoku_Grid<cell_t>&);
		Sudoku_Grid(Sudoku_Grid<cell_t>&&);
		Sudoku_Grid& operator=(const Sudoku_Grid<cell_t>&);
		Sudoku_Grid& operator=(Sudoku_Grid<cell_t>&&);
		
		void set_cell(const index_t x, const index_t y, const cell_t& c) throw (std::out_of_range);
		void set_cell(const index_t x, const index_t y, cell_t&& c) throw (std::out_of_range);
		cell_t& get_cell(const index_t x, const index_t y) throw (std::out_of_range);
		const cell_t& get_cell(const index_t x, const index_t y) const throw (std::out_of_range);
		
		unsigned int size() const {return _size;}
		unsigned int box_size() const {return _box_size;}
		bool solved() const;
		
		//! @return index of the cells box
		int get_box_index(index_t cell_x, index_t cell_y) const;
		//! @return coords of the uppper left cell in the box with index box_index
		std::pair<index_t, index_t> get_left_top_cell_coord_of_box(int box_index) const;
		
	private:
		
		grid_t _grid;
		unsigned int _size = 0;
		unsigned int _box_size = 0;
	};
	
	
	using Single_Value_Sudoku_Grid = Sudoku_Grid<Single_Value_Cell>;
	using  Multiple_Value_Sudoku_Grid = Sudoku_Grid<Multiple_Value_Cell>;
	
	template<class cell_t>
	std::ostream& operator<<(std::ostream& os, const Sudoku_Grid<cell_t>& sg);
	template<class cell_t>
	std::istream& operator>>(std::istream& is, Sudoku_Grid<cell_t>& sg);
	void print_grid(std::ostream& os, const Multiple_Value_Sudoku_Grid& g);
	
	Multiple_Value_Sudoku_Grid to_multiple_value_cell_grid(const Single_Value_Sudoku_Grid& rhs);

	template <class cell_t>
	Sudoku_Grid<cell_t>::Sudoku_Grid(unsigned int size) {
		
		unsigned box_size = (unsigned int) (sqrt(double(size)));
		
		if (box_size*box_size != size) {
			std::cout << "cannot create sudoku grid with size " << size << "(size must be a square)" << std::endl;
			return;
		}
		
		for (unsigned int  x = 0;  x < size; ++x)
			_grid.push_back(std::vector<cell_t>(size));
		
		_size = size;
		_box_size = box_size;
	}
	
	template <class cell_t>
	Sudoku_Grid<cell_t>::Sudoku_Grid(const Sudoku_Grid<cell_t>& rhs)
	: _grid(rhs._grid), _size(rhs._size), _box_size(rhs._box_size)
	{ }
	
	template <class cell_t>
	Sudoku_Grid<cell_t>::Sudoku_Grid(Sudoku_Grid<cell_t>&& rhs)
	: _grid(std::move(rhs._grid)), _size(rhs._size), _box_size(rhs._box_size)
	{
		rhs._size = 0;
		rhs._box_size = 0;
	}
	
	template <class cell_t>
	Sudoku_Grid<cell_t>& Sudoku_Grid<cell_t>::operator=(const Sudoku_Grid<cell_t>& rhs) {
		_grid = rhs._grid;
		_size = rhs._size;
		_box_size = rhs._box_size;
		
		return *this;
	}
	
	template <class cell_t>
	Sudoku_Grid<cell_t>& Sudoku_Grid<cell_t>::operator=(Sudoku_Grid<cell_t>&& rhs)
	{
		_grid = std::move(rhs._grid);
		_size = rhs._size;
		_box_size = rhs._box_size;
		rhs._size = 0;
		rhs._box_size = 0;
		
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
	
	template <class cell_t>
	bool Sudoku_Grid<cell_t>::solved() const {
		for (int x = 0; x < size(); ++x) {
			for (int y = 0; y < size(); ++y) {
				if (!get_cell(x, y).is_final())
					return false;
			}
		}
		return true;
	}
	
	
	template <class cell_t>
	int Sudoku_Grid<cell_t>::get_box_index(index_t x, index_t y) const {
		return int((x/_box_size) * _box_size + (y/_box_size)); // rounding intended!
	}
	
	template <class cell_t>
	std::pair<typename Sudoku_Grid<cell_t>::index_t, typename Sudoku_Grid<cell_t>::index_t>
	Sudoku_Grid<cell_t>::get_left_top_cell_coord_of_box(int b) const {
		int box_per_line = int(_size / _box_size);
		int b_x = b / box_per_line;
		int b_y = b % box_per_line;
		return {b_x * _box_size, b_y * _box_size};
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
				
				if (cell.max_value () > sg.size()) {
					is.setstate(std::ios::failbit);
					return is;
				}
				
				tmp.set_cell(x,y, cell);
			}
		}
		
		if (is.good())
			sg = std::move(tmp);
		return is;
	}

}

#endif /* SudokuGrid_hpp */
