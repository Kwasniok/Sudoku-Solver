//
//  solve.cpp
//  Sudoku Solver
//
//  Created by Jens Kwasniok on 05.08.16.
//  Copyright © 2016 Jens Kwasniok. All rights reserved.
//

#include "solve.h"

#include <iostream>
#include <map>
#include "util.h"

namespace sudoku_solver {
	
	//! converts the multiple valued grid into a single valued grid
	//! with all empty cells filled with all possible (1 to max_value) values
	Multiple_Value_Sudoku_Grid create_possibility_grid(const Single_Value_Sudoku_Grid& rhs, value_t max_value);

	//! special return type for _solve
	struct _solve_ret_t {
		Multiple_Value_Sudoku_Grid grid;
		bool solved=false;
		std::string reason;
	};

	//! recursive implementation of solving algoritm
	_solve_ret_t _solve(Multiple_Value_Sudoku_Grid&& mg_start,
						value_t max_value,
						std::map<int, std::vector<value_t>> boxes,
						std::map<int, std::vector<value_t>> lines_x,
						std::map<int, std::vector<value_t>> lines_y);


	
	Multiple_Value_Sudoku_Grid solve(const Single_Value_Sudoku_Grid& sg_start, value_t max_value) {
		
		// convert (single valued) sudoku grid into initial grid with multiple value support
		// this new grid contains a sigle value for each original cell with a value
		// and all possible values (e.g. 1-9 for a 9x9 grid) for each empty cell in the original
		Multiple_Value_Sudoku_Grid mg {create_possibility_grid(sg_start, max_value)};
		
		// call to recursive solving algorithm
		_solve_ret_t mg_solved = _solve(std::move(mg), max_value, {}, {}, {});
		
		// prints the reason why the solving algorithm finished
		// either 'solved' or a contradiction message (e.g. 'same value in box')
		std::cout << mg_solved.reason << std::endl;
		
		// returns the grid as multiple valued version (esp. necessary for contradictions)
		// the grid might be in an unsolved state
		return std::move(mg_solved.grid);
	}

		
	_solve_ret_t _solve(Multiple_Value_Sudoku_Grid&& mg_start,
						value_t max_value,
						std::map<int, std::vector<value_t>> boxes,
						std::map<int, std::vector<value_t>> lines_x,
						std::map<int, std::vector<value_t>> lines_y)
	{
		Multiple_Value_Sudoku_Grid mg {std::move(mg_start)};
		
		// shortcut to the box indexing function
		typename Multiple_Value_Sudoku_Grid::get_box_index_t& to_box_index = mg.get_box_index_func();
		
		// synopsis:
		//
		// step I:
		//    a:
		//    analyze grid (find unchecked cells with only one possibility)
		//    for each such cell:
		//        mark the value as used for its box and lines
		//        mark it as final
		//    b:
		//    cancel no longer possible values form all cells
		//
		// step II:
		//    for each possibility in a non-final cell:
		//        apply the solving algorithm to a copy with just one possibility cancled out
		//    return (solved or contradictory) grid
		
		// decides whether step II is applied
		bool grid_not_final;
		// tracks the amount of removed possibilities (if it differs from 0 step I is repeated)
		unsigned int removed_possibilities;
		
		// step I: analyzse and cancle possibilities
		do {
			grid_not_final = false;
			removed_possibilities = 0;
			
			// Ia: analyze cells for final values (via box and lines)
			for (int x = 0; x < mg.size_x(); ++x) {
				for (int y = 0; y < mg.size_y(); ++y) {
					
					Multiple_Value_Cell& cell = mg.get_cell(x,y);
					
					// find cell with one value left but not marked as final
					if (!cell.is_final() && cell.get_values().size() == 1) {
						
						// shortcuts to cell and sections
						value_t v = cell.get_values()[0];
						std::vector<value_t>& box = boxes[to_box_index(x,y)];
						std::vector<value_t>& line_x = lines_x[x];
						std::vector<value_t>& line_y = lines_y[y];
						
						// check for contradictions and stop if one occurred
						// or store final value as used in section and continue
						if(has_value(box, v))
							return {std::move(mg), false, "same value in box"};
						else
							box.push_back(v);
						
						if(has_value(line_x, v))
							return {std::move(mg), false, "same value in vertical line"};
						else
							line_x.push_back(v);
						
						if(has_value(line_y, v))
							return {std::move(mg), false, "same value in horizontal line"};
						else
							line_y.push_back(v);
						
						// mark cell as final
						cell.make_final();
					}
				}
			}
			
			// Ib: cancel no longer possible values
			for (int x = 0; x < mg.size_x(); ++x) {
				for (int y = 0; y < mg.size_y(); ++y) {
					
					// shortcuts to cell and its values
					Multiple_Value_Cell& cell = mg.get_cell(x,y);
					std::vector<value_t>& cell_vs = cell.get_values();
					
					// find non-final cell
					if (!cell.is_final()) {
						
						// try to remove the values stored as used in all sections of the cell
						// if the remove was possible increment the counter
						for (value_t v : boxes[to_box_index(x,y)]) {
							if (removed(cell_vs, v))
								++removed_possibilities;
						}
						for (value_t v : lines_x[x]) {
							if (removed(cell_vs, v))
								++removed_possibilities;
						}
						for (value_t v : lines_y[y]) {
							if (removed(cell_vs, v))
								++removed_possibilities;
						}
						
						// if one cell is empty the grid contains a contradiction
						if (cell.is_empty())
							return {std::move(mg), false, "no possible value for cell"};
						
						// remember if at least one cell is non-final
						if (!cell.is_final())
							grid_not_final = true;
					}
				}
			}
			
#if DEBUG
			print_grid(std::cout, mg, max_value);
			std::cout << std::endl << std::endl;
#endif
		// continue until analyzing and canceling results in no changes
		} while (removed_possibilities != 0);
		
		
		//TODO: need improved selection for assumptions
		// too much (identical) choices
		
		// apply step II if necessary or return solved grid otherwise
		if (grid_not_final) {
			
			// step II: cancle one value by pure assumption
			
			// for each possibility to cancle a non-final value
			// (cells with fewer possibilities are chosen first)
			for (int i=2; i <= max_value; ++i) {
				for (int x = 0; x < mg.size_x(); ++x) {
					for (int y = 0; y < mg.size_y(); ++y) {
						
						if (mg.get_cell(x,y).get_values().size() == i) {
							
							// for each possible value in a non-final cell
							for(value_t v : mg.get_cell(x,y).get_values()) {
								
								// copy the current grid
								Multiple_Value_Sudoku_Grid mg_assumption = mg;
								
								// remove the value
								removed(mg_assumption.get_cell(x,y).get_values(), v);
								
								// call to solving algorithm for the copy (copies used values in sections implicitly)
								_solve_ret_t ret = _solve(std::move(mg_assumption), max_value, boxes, lines_x, lines_y);
								
								// if the copied grid was solved sucessfully return the solution
								if (ret.solved)
									return ret;
							}
						}
					}
				}
			}
			
			// checked all possibilities and found no solution
			return {std::move(mg), false, "no possibility left"};
			
		} else	{
			// the grid is solved
			return {std::move(mg), true, "solved"};
		}
	}

	//! set the values of cell to all values between 1 and max_value except the current values
	Multiple_Value_Cell invert(const Single_Value_Cell& rhs, value_t max_value) {
		
		Multiple_Value_Cell inverted_tmp;
		
		for (value_t v = 1; v <= max_value; ++v)
			if (v != rhs.get_value()) inverted_tmp.add_value(v);
		
		return inverted_tmp;
	}

	Multiple_Value_Sudoku_Grid create_possibility_grid(const Single_Value_Sudoku_Grid& rhs, value_t max_value) {
		
		Multiple_Value_Sudoku_Grid tmp {rhs.size_x(), rhs.size_y(), rhs.get_box_index_func()};
		
		for (int x = 0; x < tmp.size_x(); ++x) {
			for (int y = 0; y < tmp.size_x(); ++y) {
				
				if (rhs.get_cell(x,y).is_empty()) {
					tmp.set_cell(x, y, invert(rhs.get_cell(x,y), max_value));
				} else {
					Multiple_Value_Cell mc {rhs.get_cell(x, y).get_value()};
					tmp.set_cell(x, y, mc);
				}
				
			}
		}
		return tmp;
	}
	
}
