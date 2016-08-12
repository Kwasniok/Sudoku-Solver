//
//  main.cpp
//  Sudoku Solver
//
//  Created by Jens Kwasniok on 20.06.16.
//  Copyright © 2016 Jens Kwasniok. All rights reserved.
//

#include <iostream>
#include <chrono>
#include "solve.h"

using namespace std;
using namespace sudoku_solver;

int main(int argc, const char * argv[]) {
	
	Single_Value_Sudoku_Grid sg;
	cout << "Enter your sudoku grid of dimension " << sg.size() << "x" << sg.size() << " ..." << endl;
	cout << "example of a 9x9 grid: "
		 << endl << "5 3 _ _ _ 2 1 _ 6"
		 << endl << "_ _ 7 3 _ _ _ _ _"
		 << endl << "2 _ _ _ 7 6 8 _ _"
		 << endl << "_ _ 8 5 _ _ _ 1 7"
		 << endl << "7 9 _ 4 _ 1 _ _ _"
		 << endl << "1 _ _ _ _ 8 3 _ _"
		 << endl << "8 _ _ 6 5 _ _ _ 1"
		 << endl << "_ 7 _ _ _ 4 6 _ 5"
		 << endl << "9 _ 5 1 _ _ _ 8 4" << endl << endl;

	cin >> sg;
	cout << (cin.good()? "accepted":"failed reading")  << endl << endl;
	
	if (cin.good()) {
		auto start = chrono::system_clock::now();
		Multiple_Value_Sudoku_Grid result = solve(sg);
		auto stop = chrono::system_clock::now();
		auto delta_t = chrono::duration_cast<chrono::milliseconds>(stop - start);

		if (!result.solved())
			print_grid(cout, result);
		
		cout << endl << endl << "result: (time " << delta_t.count() << "ms)" << endl << result << endl;
	}
}
