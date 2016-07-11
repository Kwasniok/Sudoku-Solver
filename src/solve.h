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
	
	template<value_t max_value>
	Multiple_Value_Sudoku_Grid<max_value> solve(const Single_Value_Sudoku_Grid<max_value>& sg_start);
	
	template<value_t max_value>
	struct _solve_ret_t {Multiple_Value_Sudoku_Grid<max_value> grid; bool solved=false; std::string reason;};
	
	template<value_t max_value>
	_solve_ret_t<max_value> _solve(Multiple_Value_Sudoku_Grid<max_value>&& mg_start, std::map<int, std::vector<value_t>> boxes, std::map<int, std::vector<value_t>> lines_x, std::map<int, std::vector<value_t>> lines_y);
	
	// auxiliar
	template<value_t max_value>
	Multiple_Value_Cell<max_value> invert(const Single_Value_Cell<max_value>& rhs);
	
	template<value_t max_value>
	Multiple_Value_Sudoku_Grid<max_value> create_possibility_grid(const Single_Value_Sudoku_Grid<max_value>& rhs);

	
	
	unsigned long branch_count = 0;
	template<value_t max_value>
	Multiple_Value_Sudoku_Grid<max_value> solve(const Single_Value_Sudoku_Grid<max_value>& sg_start) {
		Multiple_Value_Sudoku_Grid<max_value> mg {create_possibility_grid(sg_start)};
		
		branch_count = 0;
		_solve_ret_t<max_value> mg_solved = _solve(std::move(mg), {}, {}, {});
		std::cout << mg_solved.reason << std::endl;
		return std::move(mg_solved.grid);
	}
	
	template<value_t max_value>
	_solve_ret_t<max_value> _solve(Multiple_Value_Sudoku_Grid<max_value>&& mg_start, std::map<int, std::vector<value_t>> boxes, std::map<int, std::vector<value_t>> lines_x, std::map<int, std::vector<value_t>> lines_y) {
		Multiple_Value_Sudoku_Grid<max_value> mg {std::move(mg_start)};
		
		const typename Multiple_Value_Sudoku_Grid<max_value>::get_box_index_t& to_box_index = mg.get_box_index_func();
		
		bool cont = true;
		unsigned int changes = 1;
		while (changes != 0) {
			
			// Ia: analyze cells for final values (via box and lines)
			for (int x = 0; x < mg.size_x(); ++x) {
				for (int y = 0; y < mg.size_y(); ++y) {
					
					Multiple_Value_Cell<max_value>& cell = mg.get_cell(x,y);
					
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
					
					Multiple_Value_Cell<max_value>& cell = mg.get_cell(x,y);
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
						
						//std::cout << mg << std::endl << std::endl;
						//std::cout.flush();
					}
				}
			}
			
			if (cont && changes == 0) {
				for (int i=2; i <= max_value; ++i) {
					for (int x = 0; x < mg.size_x(); ++x) {
						for (int y = 0; y < mg.size_y(); ++y) {
							if (mg.get_cell(x,y).get_values().size() == i) {
								for(value_t v : mg.get_cell(x,y).get_values()) {
									Multiple_Value_Sudoku_Grid<max_value> mg_assumption = mg;
									removed(mg_assumption.get_cell(x,y).get_values(), v);
									
									++branch_count;
									if (branch_count % 10000 == 0) std::cout << branch_count << std::endl;
									_solve_ret_t<max_value> ret = _solve(std::move(mg_assumption), boxes, lines_x, lines_y);
									
									if (ret.solved)
										return std::move(ret);
								}
							}
						}
					}
				}
			}
		}
		
		return {std::move(mg), true, "solved"};
	}

	template<value_t max_value>
	Multiple_Value_Cell<max_value> invert(const Single_Value_Cell<max_value>& rhs) {
		Multiple_Value_Cell<max_value> inverted_tmp;
		for (value_t v = 1; v <= max_value; ++v)
			if (v != rhs.get_value()) inverted_tmp.add_value(v);
		return inverted_tmp;
	}
	
	template<value_t max_value>
	Multiple_Value_Sudoku_Grid<max_value> create_possibility_grid(const Single_Value_Sudoku_Grid<max_value>& rhs) {
		
		Multiple_Value_Sudoku_Grid<max_value> tmp {rhs.size_x(), rhs.size_y(), rhs.get_box_index_func()};
		
		for (int x = 0; x < tmp.size_x(); ++x) {
			for (int y = 0; y < tmp.size_x(); ++y) {
				
				if (rhs.get_cell(x,y).is_empty()) {
					tmp.set_cell(x, y, invert(rhs.get_cell(x,y)));
				} else {
					Multiple_Value_Cell<max_value> mc {rhs.get_cell(x, y).get_value()};
					tmp.set_cell(x, y, mc);
				}
				
			}
		}
		return tmp;
	}
	
}


#endif /* solve_h */
