# Sudoku-Solver
a simple sudoku solver

## Installation
- [download](https://github.com/Kwasniok/Sudoku-Solver/archive/master.zip) folder or check out this repository
- open console and change directory to downloaded folder `cd <path to repo folder>/src`
- run `make` command
- run programm via `./main`

## Usage
- enter the dimension of your sudoku grid (only square numbers are allowed)
- enter your sudoku grid
  - mark empty cells with an underscore `_`
  - leave white space between cells
  - make sure your grid is a valid one
  - structuring your grid in lines is optional
  - finish your grid with some white space (e.g. hit `enter`)
- the program indicates weather your input was accepted or not (check last two steps if it's not accepted)
- the program returns its result besides a short description if the grid was solved or if some contradiction appeared during it's attempt to solve it
- finally the result is presented
  - if there was no solution found the intermediate sate (in which the contradition appeared) is presented instead
  - only one solution will be returned if the grid is unambiguous

