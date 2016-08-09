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
						int box_size,
						std::map<int, std::vector<value_t>> boxes,
						std::map<int, std::vector<value_t>> lines_x,
						std::map<int, std::vector<value_t>> lines_y);


	
	Multiple_Value_Sudoku_Grid solve(const Single_Value_Sudoku_Grid& sg_start, value_t max_value, int box_size) {
		
		// convert (single valued) sudoku grid into initial grid with multiple value support
		// this new grid contains a sigle value for each original cell with a value
		// and all possible values (e.g. 1-9 for a 9x9 grid) for each empty cell in the original
		Multiple_Value_Sudoku_Grid mg {create_possibility_grid(sg_start, max_value)};
		
		// call to recursive solving algorithm
		_solve_ret_t mg_solved = _solve(std::move(mg), max_value, box_size, {}, {}, {});
		
		// prints the reason why the solving algorithm finished
		// either 'solved' or a contradiction message (e.g. 'same value in box')
		std::cout << mg_solved.reason << std::endl;
		
		// returns the grid as multiple valued version (esp. necessary for contradictions)
		// the grid might be in an unsolved state
		return std::move(mg_solved.grid);
	}
	
	int to_box_index(long x, long y, int box_size) {
		return int((x/box_size) * box_size + (y/box_size)); // rounding intended!
	}

	//! returns the coords of the uppper left cell in the box with index b
	std::pair<int, int> box_to_cell_coords(int b, int box_size, long grid_size) {
		int box_per_line = int(grid_size / box_size);
		int b_x = b / box_per_line;
		int b_y = b % box_per_line;
		return {b_x * box_size, b_y * box_size};
	}

		
	_solve_ret_t _solve(Multiple_Value_Sudoku_Grid&& mg_start,
						value_t max_value,
						int box_size,
						std::map<int, std::vector<value_t>> boxes,
						std::map<int, std::vector<value_t>> lines_x,
						std::map<int, std::vector<value_t>> lines_y)
	{
		Multiple_Value_Sudoku_Grid mg {std::move(mg_start)};
		
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
			
			// Ia.a: find cell with one value left but not marked as final
			for (int x = 0; x < mg.size_x(); ++x) {
				for (int y = 0; y < mg.size_y(); ++y) {
					
					Multiple_Value_Cell& cell = mg.get_cell(x,y);
					
					if (!cell.is_final() && cell.get_values().size() == 1) {
						
						// shortcuts to cell and sections
						value_t v = cell.get_values()[0];
						std::vector<value_t>& box = boxes[to_box_index(x, y, box_size)];
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
			
			// Ia.b: find single occurrence of a value in a section
			
			// for each value
			for (value_t v = 1; v <= max_value; ++v) {
				
				bool value_occurred_once;
				int cell_pos_x, cell_pos_y;
				
				// by vertical line
				for (int x = 0; x < mg.size_x(); ++x) {
					
					if (!has_value(lines_x[x], v)) {
						
						value_occurred_once = false;
						
						for (int y = 0; y < mg.size_y(); ++y) {
							
							if (has_value(mg.get_cell(x, y).get_values(), v)) {
								
								if (value_occurred_once) {value_occurred_once = false; break;} // occurred twice
								value_occurred_once = true;
								cell_pos_x = x;
								cell_pos_y = y;
								
							}
						}
						
						if (value_occurred_once) {
								
							// shortcuts to cell & sections
							Multiple_Value_Cell& c = mg.get_cell(cell_pos_x, cell_pos_y);
							std::vector<value_t>& box = boxes[to_box_index(cell_pos_x, cell_pos_y, box_size)];
							std::vector<value_t>& line_x = lines_x[cell_pos_x];
							std::vector<value_t>& line_y = lines_y[cell_pos_y];
							
							// check for contradictions and stop if one occurred
							// or store final value as used in section and continue
							if(has_value(box, v))
								return {std::move(mg), false, "same value in box"};
							else
								box.push_back(v);
							
							line_x.push_back(v);
							
							if(has_value(line_y, v))
								return {std::move(mg), false, "same value in horizontal line"};
							else
								line_y.push_back(v);
							
							// cancel all other possible values for this cell
							c.set_values({v});
							// mark cell as final
							c.make_final();
						}
						
					}
					
				}
				
				// by horizontal line
				for (int y = 0; y < mg.size_y(); ++y) {
					
					if (!has_value(lines_y[y], v)) {
						
						value_occurred_once = false;
						
						for (int x = 0; x < mg.size_x(); ++x) {
							
							if (has_value(mg.get_cell(x, y).get_values(), v)) {
								
								if (value_occurred_once) {value_occurred_once = false; break;} // occurred twice
								value_occurred_once = true;
								cell_pos_x = x;
								cell_pos_y = y;
								
							}
						}
						
						if (value_occurred_once) {
							
							// shortcuts to cell & sections
							Multiple_Value_Cell& c = mg.get_cell(cell_pos_x, cell_pos_y);
							std::vector<value_t>& box = boxes[to_box_index(cell_pos_x, cell_pos_y, box_size)];
							std::vector<value_t>& line_x = lines_x[cell_pos_x];
							std::vector<value_t>& line_y = lines_y[cell_pos_y];
							
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

								line_y.push_back(v);
							
							// cancel all other possible values for this cell
							c.set_values({v});
							// mark cell as final
							c.make_final();
						}
						
					}
					
				}
				
				// by box
				for (int b = 0; b < (mg.size_x() / box_size) * (mg.size_x() / box_size) ; ++b) {
					
					if (!has_value(boxes[b], v)) {
						
						value_occurred_once = false;
						
						std::pair<int, int> c_coord_box = box_to_cell_coords(b, box_size, mg.size_x());
						int x_box = c_coord_box.first;
						int y_box = c_coord_box.second;
						
						bool brk = false; // brk (break)
						for (int x_rel = 0; !brk && x_rel < box_size; ++x_rel) {
							for (int y_rel = 0; y_rel < box_size; ++y_rel) {
						
							
								if (has_value(mg.get_cell(x_box + x_rel, y_box + y_rel).get_values(), v)) {
									
									if (value_occurred_once) {value_occurred_once = false; brk = true; break;} // occurred twice
									value_occurred_once = true;
									cell_pos_x = x_box + x_rel;
									cell_pos_y = y_box + y_rel;
									
								}
							}
						}
						
						if (value_occurred_once) {
							
							// shortcuts to cell & sections
							Multiple_Value_Cell& c = mg.get_cell(cell_pos_x, cell_pos_y);
							std::vector<value_t>& box = boxes[to_box_index(cell_pos_x, cell_pos_y, box_size)];
							std::vector<value_t>& line_x = lines_x[cell_pos_x];
							std::vector<value_t>& line_y = lines_y[cell_pos_y];
							
							// check for contradictions and stop if one occurred
							// or store final value as used in section and continue
							box.push_back(v);
							
							if(has_value(line_x, v))
								return {std::move(mg), false, "same value in vertical line"};
							else
								line_x.push_back(v);
							
							if(has_value(line_y, v))
								return {std::move(mg), false, "same value in horizontal line"};
							else
								line_y.push_back(v);
							
							// cancel all other possible values for this cell
							c.set_values({v});
							// mark cell as final
							c.make_final();
						}
						
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
						for (value_t v : boxes[to_box_index(x, y, box_size)]) {
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
								_solve_ret_t ret = _solve(std::move(mg_assumption), max_value, box_size, boxes, lines_x, lines_y);
								
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
		
		Multiple_Value_Sudoku_Grid tmp {rhs.size_x(), rhs.size_y()};
		
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
