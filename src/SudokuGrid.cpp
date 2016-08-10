//
//  SudokuGrid.cpp
//  Sudoku Solver
//
//  Created by Jens Kwasniok on 24.07.16.
//  Copyright © 2016 Jens Kwasniok. All rights reserved.
//

#include "SudokuGrid.h"
#include "util.h"


using namespace sudoku_solver;

void sudoku_solver::print_grid(std::ostream& os, const Multiple_Value_Sudoku_Grid& g, const value_t max_value) {
	bool first_x = true;
	for (int x = 0; x < g.size(); ++x) {
		if (first_x) first_x = false;
		else os << std::endl;
		
		bool first_y = true;
		for (int y = 0; y < g.size(); ++y) {
			if(first_y) first_y = false;
			else os << ' ';
			
			os << '[';
			for (value_t v=1; v <= max_value; ++v) {
				const Multiple_Value_Cell& c = g.get_cell(x,y);
				if (has_value(c.get_values(), v))
					os << v;
				else
					os << ' ';
			}
			os << ']';
		}
	}
}

Multiple_Value_Sudoku_Grid sudoku_solver::to_multiple_value_cell_grid(const Single_Value_Sudoku_Grid& rhs) {
	Multiple_Value_Sudoku_Grid tmp {rhs.size()};
	for (int x = 0; x < tmp.size(); ++x) {
		for (int y = 0; y < tmp.size(); ++y) {
			tmp.set_cell(x, y, {rhs.get_cell(x,y).get_value()});
		}
	}
	return tmp;
}
