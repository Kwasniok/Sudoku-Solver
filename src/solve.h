//
//  solve.h
//  Sudoku Solver
//
//  Created by Jens Kwasniok on 21.06.16.
//  Copyright © 2016 Jens Kwasniok. All rights reserved.
//

#ifndef solve_h
#define solve_h

#include <iostream>
#include <map>
#include "SudokuGrid.hpp"
#include "util.h"

namespace sudoku_solver {
	
	//! tries to slove the given sudoku
	//! @return either the solved sudoku or a sudoku gird in a state in which the algorith detected a contradiction
	Multiple_Value_Sudoku_Grid solve(const Single_Value_Sudoku_Grid& sg_start, value_t max_value=9);
	
	namespace {
		struct _solve_ret_t {Multiple_Value_Sudoku_Grid grid; bool solved=false; std::string reason;};
		_solve_ret_t _solve(Multiple_Value_Sudoku_Grid&& mg_start, value_t max_value, std::map<int, std::vector<value_t>> boxes, std::map<int, std::vector<value_t>> lines_x, std::map<int, std::vector<value_t>> lines_y);
		Multiple_Value_Sudoku_Grid create_possibility_grid(const Single_Value_Sudoku_Grid& rhs, value_t max_value);
		unsigned long branch_count = 0;
	}
	

	Multiple_Value_Sudoku_Grid solve(const Single_Value_Sudoku_Grid& sg_start, value_t max_value) {
		Multiple_Value_Sudoku_Grid mg {create_possibility_grid(sg_start, max_value)};
		
		branch_count = 0;
		_solve_ret_t mg_solved = _solve(std::move(mg), max_value, {}, {}, {});
		std::cout << mg_solved.reason << std::endl;
		return std::move(mg_solved.grid);
	}
	
	namespace {
		
		_solve_ret_t _solve(Multiple_Value_Sudoku_Grid&& mg_start, value_t max_value, std::map<int, std::vector<value_t>> boxes, std::map<int, std::vector<value_t>> lines_x, std::map<int, std::vector<value_t>> lines_y) {
			Multiple_Value_Sudoku_Grid mg {std::move(mg_start)};
			
			typename Multiple_Value_Sudoku_Grid::get_box_index_t& to_box_index = mg.get_box_index_func();
			
			bool cont = true;
			unsigned int changes = 1;
			while (changes != 0) {
				
				// Ia: analyze cells for final values (via box and lines)
				for (int x = 0; x < mg.size_x(); ++x) {
					for (int y = 0; y < mg.size_y(); ++y) {
						
						Multiple_Value_Cell& cell = mg.get_cell(x,y);
						
						// if just one value left but not marked final yet
						if (!cell.is_final() && cell.get_values().size() == 1) {
							
							value_t v = cell.get_values()[0];
							std::vector<value_t>& box = boxes[to_box_index(x,y)];
							std::vector<value_t>& line_x = lines_x[x];
							std::vector<value_t>& line_y = lines_y[y];
							
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
							
							cell.make_final();
						}
					}
				}
				
				// Ib: cancel no longer possible values
				cont = false;
				changes = 0;
				for (int x = 0; x < mg.size_x(); ++x) {
					for (int y = 0; y < mg.size_y(); ++y) {
						
						Multiple_Value_Cell& cell = mg.get_cell(x,y);
						std::vector<value_t>& cell_vs = cell.get_values();
						
						if (!cell.is_final()) {
							
							for (value_t v : boxes[to_box_index(x,y)]) {
								if (removed(cell_vs, v))
									++changes;
							}
							for (value_t v : lines_x[x]) {
								if (removed(cell_vs, v))
									++changes;
							}
							for (value_t v : lines_y[y]) {
								if (removed(cell_vs, v))
									++changes;
							}
							
							if (cell.is_empty())
								return {std::move(mg), false, "no possible value for cell"};
							if (!cell.is_final())
								cont = true; // at least one cell is not final
						}
					}
				}
			}
			
			
			//TODO: need improved selection for assumptions
			// too much (identical) choices
			// II: cancle one value by pure assumption
			while (cont) {
				for (int i=2; i <= max_value; ++i) {
					for (int x = 0; x < mg.size_x(); ++x) {
						for (int y = 0; y < mg.size_y(); ++y) {
							
							if (mg.get_cell(x,y).get_values().size() == i) {
								
								for(value_t v : mg.get_cell(x,y).get_values()) {
									
									Multiple_Value_Sudoku_Grid mg_assumption = mg;
									removed(mg_assumption.get_cell(x,y).get_values(), v); // remove one value
									
									++branch_count;
									if (branch_count % 10000 == 0) std::cout << branch_count << std::endl;
									
									_solve_ret_t ret = _solve(std::move(mg_assumption), max_value, boxes, lines_x, lines_y);
									
									if (ret.solved)
										return std::move(ret);
								}
							}
						}
					}
				}
				
				cont = false;
				for (int x = 0; !cont && x < mg.size_x(); ++x) {
					for (int y = 0; !cont && y < mg.size_y(); ++y) {
						if (!mg.get_cell(x,y).is_final()) {
							cont = true;
						}
					}
				}
			}
			
			return {std::move(mg), true, "solved"};
		}

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
}


#endif /* solve_h */
