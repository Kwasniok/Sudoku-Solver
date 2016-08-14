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
	Multiple_Value_Sudoku_Grid create_possibility_grid(const Single_Value_Sudoku_Grid& rhs);

	//! special return type for _solve
	struct _solve_ret_t {
		Multiple_Value_Sudoku_Grid grid;
		bool solved=false;
		std::string reason;
	};

	//! recursive implementation of solving algoritm
	_solve_ret_t _solve(Multiple_Value_Sudoku_Grid&& mg_start,
						std::map<int, std::vector<value_t>> boxes,
						std::map<int, std::vector<value_t>> lines_x,
						std::map<int, std::vector<value_t>> lines_y);


	
	Multiple_Value_Sudoku_Grid solve(const Single_Value_Sudoku_Grid& sg_start) {
		
		// convert (single valued) sudoku grid into initial grid with multiple value support
		// this new grid contains a sigle value for each original cell with a value
		// and all possible values (e.g. 1-9 for a 9x9 grid) for each empty cell in the original
		Multiple_Value_Sudoku_Grid mg {create_possibility_grid(sg_start)};
		
		// call to recursive solving algorithm
		_solve_ret_t mg_solved = _solve(std::move(mg), {}, {}, {});
		
		// prints the reason why the solving algorithm finished
		// either 'solved' or a contradiction message (e.g. 'same value in box')
		std::cout << mg_solved.reason << std::endl;
		
		// returns the grid as multiple valued version (esp. necessary for contradictions)
		// the grid might be in an unsolved state
		return std::move(mg_solved.grid);
	}
		
	_solve_ret_t _solve(Multiple_Value_Sudoku_Grid&& mg_start,
						std::map<int, std::vector<value_t>> boxes,
						std::map<int, std::vector<value_t>> lines_x,
						std::map<int, std::vector<value_t>> lines_y)
	{
		Multiple_Value_Sudoku_Grid mg {std::move(mg_start)};
		
		// synopsis:
		//
		// step I: (analytical part)
		//
		//    a: find final cells
		//    a.a: find unchecked cells with only one possibility
		//        for each such cell:
		//            mark the value as used for its box and lines
		//            mark it as final
		//    a.b: find single occurrence of a value in a section
		//        makr those cells as final
		//    both sub-steps might return with a contradiction
		//
		//    b: cancel possibilities
		//    b.a: cancel no longer possible values form all cells
		//    b.b: cancles possibilities due to 'blocking' of a value whose position in one box is fixed to one line
		//
		//
		// step II: (assumption based part)
		//
		//    for each possibility in a non-final cell:
		//        apply the solving algorithm to a copy with just one possibility cancled out
		//    return (solved or contradictory) grid
		//
		
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
			for (int x = 0; x < mg.size(); ++x) {
				for (int y = 0; y < mg.size(); ++y) {
					
					Multiple_Value_Cell& cell = mg.get_cell(x,y);
					
					if (!cell.is_final() && cell.possibilities() == 1) {
						
						// shortcuts to cell and sections
						value_t v = cell.get_values()[0];
						std::vector<value_t>& box = boxes[mg.get_box_index(x, y)];
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
			for (value_t v = 1; v <= mg.size(); ++v) {
				
				bool value_occurred_once;
				int cell_pos_x, cell_pos_y;
				
				// by vertical line
				for (int x = 0; x < mg.size(); ++x) {
					
					if (!has_value(lines_x[x], v)) {
						
						value_occurred_once = false;
						
						for (int y = 0; y < mg.size(); ++y) {
							
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
							std::vector<value_t>& box = boxes[mg.get_box_index(cell_pos_x, cell_pos_y)];
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
				for (int y = 0; y < mg.size(); ++y) {
					
					if (!has_value(lines_y[y], v)) {
						
						value_occurred_once = false;
						
						for (int x = 0; x < mg.size(); ++x) {
							
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
							std::vector<value_t>& box = boxes[mg.get_box_index(cell_pos_x, cell_pos_y)];
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
				for (int b = 0; b < mg.size() ; ++b) {
					
					if (!has_value(boxes[b], v)) {
						
						value_occurred_once = false;
						
						std::pair<int, int> c_coord_box = mg.get_left_top_cell_coord_of_box(b);
						int x_box = c_coord_box.first;
						int y_box = c_coord_box.second;
						
						bool brk = false; // brk (break)
						for (int x_rel = 0; !brk && x_rel < mg.box_size(); ++x_rel) {
							for (int y_rel = 0; y_rel < mg.box_size(); ++y_rel) {
						
							
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
							std::vector<value_t>& box = boxes[mg.get_box_index(cell_pos_x, cell_pos_y)];
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
			
			// Ib.a: cancel no longer possible values (due to final values)
			for (int x = 0; x < mg.size(); ++x) {
				for (int y = 0; y < mg.size(); ++y) {
					
					// shortcuts to cell and its values
					Multiple_Value_Cell& cell = mg.get_cell(x,y);
					std::vector<value_t>& cell_vs = cell.get_values();
					
					// find non-final cell
					if (!cell.is_final()) {
						
						// try to remove the values stored as used in all sections of the cell
						// if the remove was possible increment the counter
						for (value_t v : boxes[mg.get_box_index(x, y)]) {
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
			
			// Ib.b cancles possibilities due to 'blocking' of a value whose position in one box is fixed to one line
			// for each box
			for (int b_x = 0; b_x < mg.box_size(); ++b_x) {
				for (int b_y = 0; b_y < mg.box_size(); ++b_y) {
					
					std::vector<std::vector<value_t>> line_x_rel (mg.box_size());
					std::vector<std::vector<value_t>> line_y_rel (mg.box_size());
					
					// for each cell in this box (coords relative to top left cell in box)
					for (int x_rel = 0; x_rel < mg.box_size(); ++x_rel) {
						for (int y_rel = 0; y_rel < mg.box_size(); ++y_rel) {
							
							Multiple_Value_Cell& c = mg.get_cell(b_x * mg.box_size() + x_rel, b_y * mg.box_size() + y_rel);
							
							for (value_t v : c.get_values()) {
								
								if (!has_value(line_x_rel[x_rel], v))
									line_x_rel[x_rel].push_back(v);
								
								if (!has_value(line_y_rel[y_rel], v))
									line_y_rel[y_rel].push_back(v);
							}
						}
					}
					
					// histogramms to analyze the frequency of a value mapped onto lines
					std::map<value_t, int> histogramm_x;
					std::map<value_t, int> histogramm_y;
					
					// for each line increment the fequency for every such value
					for (int i_rel = 0; i_rel < mg.box_size(); ++i_rel) {
						for (value_t v : line_x_rel[i_rel])
							++histogramm_x[v];
						for (value_t v : line_y_rel[i_rel])
							++histogramm_y[v];
					}
					
					// find values which occurre in only one line (frequency of one)
					// and store them
					std::vector<value_t> single_values_x;
					std::vector<value_t> single_values_y;
					for (auto& v : histogramm_x) {
						if (v.second == 1)
							single_values_x.push_back(v.first);
					}
					for (auto& v : histogramm_y) {
						if (v.second == 1)
							single_values_y.push_back(v.first);
					}
					
					// find values of frequency one in each line and remove the possibility along this line of this value
					// except the possibilities inside the current box:
					
					// for each line
					for (int i_rel = 0; i_rel < mg.box_size(); ++i_rel) {
						
						// x-direction
						// if the values occured in this line
						for (value_t v : single_values_x) {
							if (has_value(line_x_rel[i_rel], v)) {
								
								// for this line remove v from other boxes on this line
								for (int i = 0; i < mg.box_size(); ++i) {
									if (i == i_rel) continue;
									
									// for each cell in this line except the cells in this box
									for (int y = 0; y < mg.size(); ++y) {
										if (b_y * mg.box_size() <= y && y < (b_y + 1) * mg.box_size()) continue;
										
										std::vector<value_t>& vs = mg.get_cell(b_x * mg.box_size() + i_rel, y).get_values();
										remove(vs, v);
										
									}
								}
							}
						}
						
						// y-direction
						// if the values occured in this line
						for (value_t v : single_values_y) {
							if (has_value(line_y_rel[i_rel], v)) {
								
								// for this line remove v from other boxes on this line
								for (int i = 0; i < mg.box_size(); ++i) {
									if (i == i_rel) continue;
									
									// for each cell in this line except the cells in this box
									for (int x = 0; x < mg.size(); ++x) {
										if (b_x * mg.box_size() <= x && x < (b_x + 1) * mg.box_size()) continue;
										
										std::vector<value_t>& vs = mg.get_cell(x, b_y * mg.box_size() + i_rel).get_values();
										remove(vs, v);
										
									}
								}
							}
						}
						
					}
					
				}
			}
			
#if DEBUG
			print_grid(std::cout, mg);
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
			for (int i=2; i <= mg.size(); ++i) {
				for (int x = 0; x < mg.size(); ++x) {
					for (int y = 0; y < mg.size(); ++y) {
						
						if (mg.get_cell(x,y).possibilities() == i) {
							
							// for each possible value in a non-final cell
							for(value_t v : mg.get_cell(x,y).get_values()) {
								
								// copy the current grid
								Multiple_Value_Sudoku_Grid mg_assumption = mg;
								
								// remove the value
								removed(mg_assumption.get_cell(x,y).get_values(), v);
								
								// call to solving algorithm for the copy (copies used values in sections implicitly)
								_solve_ret_t ret = _solve(std::move(mg_assumption), boxes, lines_x, lines_y);
								
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

	Multiple_Value_Sudoku_Grid create_possibility_grid(const Single_Value_Sudoku_Grid& rhs) {
		
		Multiple_Value_Sudoku_Grid tmp {rhs.size()};
		
		for (int x = 0; x < tmp.size(); ++x) {
			for (int y = 0; y < tmp.size(); ++y) {
				
				if (rhs.get_cell(x,y).is_empty()) {
					tmp.set_cell(x, y, invert(rhs.get_cell(x,y), rhs.size()));
				} else {
					Multiple_Value_Cell mc {rhs.get_cell(x, y).get_value()};
					tmp.set_cell(x, y, mc);
				}
				
			}
		}
		return tmp;
	}
	
}
