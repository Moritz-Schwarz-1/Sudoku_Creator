# Sudoku_Creator
## About
This is a small project to sharpen my C skills. 
The program will create a number of sudokus specified by the user and store it in `sudokus.txt` in the same ways as in `examples.txt`.
The Algorithm will always try to remove all 81 numbers from a sudoku - refilling the hole if uniqueness is lost - to make the puzzles as challenging as possible.

## Getting started
1. Clone this repo into your desired directory <br> `git clone <repository-url> <destination>`
2. Navigate to the project directory <br> `cd sudoku_creator`
3. Compile the program using make <br> `make`
4. Run the program <br> `./sudoku_creator`

## How to read the sudokus.txt file?
The Program pastes each sodoku into a single line followed by the amount of empty cells in the corresponding puzzle after a #. The puzzles are converted row by row into the single line. 


## Requirements

* GCC <br>
* GNU Make


