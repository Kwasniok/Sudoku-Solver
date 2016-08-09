//
//  solve.h
//  Sudoku Solver
//
//  Created by Jens Kwasniok on 21.06.16.
//  Copyright © 2016 Jens Kwasniok. All rights reserved.
//

#ifndef solve_h
#define solve_h

#include "SudokuGrid.h"

namespace sudoku_solver {
	
	//! tries to slove the given sudoku
	//! @return either the solved sudoku or a sudoku gird in a state in which the algorith detected a contradiction
	Multiple_Value_Sudoku_Grid solve(const Single_Value_Sudoku_Grid& sg_start, value_t max_value=9, int box_size=3);
}

#endif /* solve_h */
