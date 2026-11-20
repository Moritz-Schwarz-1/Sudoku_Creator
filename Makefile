sudoku_creator: src/sudoku_creator.o
	gcc src/sudoku_creator.o -o sudoku_creator -O2

src/sudoku_creator.o: src/sudoku_creator.c
	gcc src/sudoku_creator.c -c -o src/sudoku_creator.o

clean:
	rm src/sudoku_creator.o sudoku_creator -f