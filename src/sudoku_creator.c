#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define PROGRESSBAR_WIDTH 25 // Amount of characters printed in a progressbar

int solve_sudoku(int sudoku[81], int square_index);

int solve_sudoku_randomly(int sudoku[81], int square_index);

int count_solutions(int sudoku[81], int square_index);

int validate(int sudoku[], int square_index);

void print_sudoku(int sudoku[81]);

void print_sudoku_oneline(int sudoku[81]);

int *create_sudoku(int difficulty);

int failed_hole_numbers[81] = {0}; //TODO Make nicer
int holes_created = 0;

int main(int argc, char *argv[]) {

    FILE* fp;
    fp = fopen("sudokus.txt","r");
    unsigned int sudokus_amount = 50;
    unsigned int difficulty = 81;
    int numbering = 1; // determines whether #XX follows sudokus

    if (argc > 1) {
        int conflicts[3] = {}; // Keep track of conflicting arguments

        for (int arguments = 1; arguments < argc; arguments++) {
            //printf(">%s<",argv[arguments]);


            // Reading argv[]

            if (!strcmp( argv[arguments], "--append" ) ) {
                fp = fopen("sudokus.txt","a");
                conflicts[0]++;
            }

            else if (!strcmp( argv[arguments], "--replace")) {
                fp = fopen("sudokus.txt","w");
                conflicts[0]++;
            }

            else if (!strcmp( argv[arguments], "--help") || !strcmp( argv[arguments], "-h")) {
                printf("\nSyntax: ./sudoku_creator [Flags,...]\n"
                       "\nFlags:\n"
                       "if sudokus.txt already exists, specify ONE of:\n"
                       "--append : append sudokus.txt.\n"
                       "--replace : delete contents of sudokus.txt.\n"
                       "\n"
                       "--set-max-diff [VALUE] : sets how many numbers the creator tries to remove. Accepts nonnegative integers and truncates them to 81 if too big. (Standard: 81)\n"
                       "--set-amount [VALUE] : sets how many sudokus should be created. Accepts nonnegative integers. (Standard: 50)\n"
                       "\n"
                       "--no-numbering : Disables the #XX after the sudokus in sudokus.txt, which stores the difficulty of a sudoku.\n"
                       "\n"
                       "--help or -h : prints all Flags\n"
                       "");
                return 0;
            }

            else if (!strcmp( argv[arguments], "--set-max-diff") ) {
                if (++arguments > argc) {
                    printf("ERROR: \"--set-max-diff didnt get a parameter\"\n");
                    return 3;
                }
                conflicts[1]++;

                difficulty = atoi(argv[arguments]);
                if (difficulty > 81) {
                    printf("WARNING: got difficulty > 81, truncating it down to 81\n");
                    difficulty = 81;
                }
                if (difficulty == 0)
                    printf("WARNING: got difficulty == 0, if this is not wanted, specify a parameter!\n");
            }

            else if (!strcmp( argv[arguments], "--set-amount") ) {
                if (++arguments > argc) {
                    printf("ERROR: \"--set-amount didnt get a parameter\"\n");
                    return 3;
                }
                conflicts[2]++;

                sudokus_amount = atoi(argv[arguments]);
                if (sudokus_amount == 0) {
                    printf("ERROR: got invalid value for \"--set-amount\"\n");
                    return 3;
                }
            }

            else if (!strcmp( argv[arguments], "--no-numbering")) {
                numbering = 0;
            }

            else {
                printf("ERROR: unknown argument: %s\n", argv[arguments]);
                return 3;
            }
        }

        // Check if conflict

        if (conflicts[0] > 1) {
            printf("ERROR: got contradicting arguments \"--replace\" and \"--append\"\n");
            return 3;
        }
        if (conflicts[1] > 1) {
            printf("ERROR: got contradicting parameters for \"--set-max-diff\"\n");
            return 3;
        }
        if (conflicts[2] > 1) {
            printf("ERROR: got contradicting parameters for \"-set-amount\"\n");
            return 3;
        }

        if (fp != NULL && conflicts[0] == 0) {
            printf("\nERROR: sudokus.txt already exists. use \"--append\" or \"--replace\" to specify what to do!\n");
            return 3;
        }

        if (fp == NULL && conflicts[0] == 0) {
            fp = fopen("sudokus.txt","w");
        }
    }


    else {
        if (fp != NULL) {
            char ch;
            printf("sudokus.txt already exists in this directory. The creator will write content into this file. Is this ok? [y/n]? ");
            do
                ch = getchar();
            while (ch != 'y' && ch != 'Y' && ch != 'n' && ch != 'N');

            if (ch == 'n' || ch == 'N') {
                return 1;
            }
        }

        fclose(fp);
        fp = fopen("sudokus.txt","a");

        if (fp == NULL) {
            printf("Error opening sudokus.txt\n");
            return 1;
        }

        printf("\nHow many Sudokus should be created?: ");
        while (scanf("%d",&sudokus_amount) == 0) {
            printf("Invalid input! Please Enter a valid Number: ");
            while (getchar() != '\n');  //Flushing buffer
        }
    }

    srand(time(NULL));
    int* sudoku;
    int sudokus_per_holes[81]= {0};

        failed_hole_numbers[55] = 0;
        clock_t start = clock();

        for (int i = 1; i <= sudokus_amount; i++) {

            printf("\r[");
            int progress_bar_elements = (float) i / sudokus_amount * PROGRESSBAR_WIDTH;
            int k;
            for (k = 0; k < progress_bar_elements; k++) {

                printf("=");
            }
            for (; k < PROGRESSBAR_WIDTH; k++) {
                printf(" ");
            }
            printf("]");
            printf(" [ %.2f%% ]",  i * 100.0f / sudokus_amount  );
            fflush(stdout);

            sudoku = create_sudoku(difficulty);

            if (sudoku == NULL) {
                printf("\nError allocating memory for a sudoku\n");
                return 2;
            }
            for (int j = 0; j < 81; j++) {
                fprintf(fp,"%d",sudoku[j]);
            }
            if (numbering == 1) {
                fprintf(fp," #%d",holes_created);
            }
            fprintf(fp,"\n");
            free(sudoku);

            sudokus_per_holes[holes_created]++;
            holes_created = 0;
        }

        printf("\n\nGenerated %d sudokus, Results:\n\n",sudokus_amount);
        for (int i = 0; i < 81; i++) {

            if (sudokus_per_holes[i] != 0) {
                printf("#Holes: %d   %-5d Sudokus\n",i,sudokus_per_holes[i]);
            }
        }

        printf("\nAnd took %lf seconds", ((double)clock() - start) / CLOCKS_PER_SEC);
        printf("\n= %lf seconds per created sudoku\n", ((double)clock() - start) / (sudokus_amount * CLOCKS_PER_SEC));

    fclose(fp);
    return 0;
}


int solve_sudoku(int sudoku[81], int square_index) {

    if (square_index == 81) {
        printf("  SOLVED!\n");
        return 0;
    }

    int x = square_index % 9;
    int y = square_index / 9;

    if (sudoku[y * 9 + x] == 0) {

        for (int i = 1; i <= 9; i++) {

            sudoku[y * 9 + x] = i;

            if (!validate( sudoku, square_index) ) {
                if (!solve_sudoku(sudoku, square_index + 1))
                    return 0;
            }
        }
        sudoku[y * 9 + x] = 0;
    }
    else {
        if (!solve_sudoku(sudoku, square_index + 1))
            return 0;
    }

    return 1;
}


int count_solutions(int sudoku[81], int square_index) { //TODO: find better order for holes to be checked

    int solutions_count = 0;

    if (square_index == 81) {
        return 1;
    }

    int x = square_index % 9;
    int y = square_index / 9;

    if (sudoku[ y * 9 + x ] == 0) {

        for (int i = 1; i <= 9; i++) {

            sudoku[y * 9 + x] = i;
            if (!validate( sudoku, square_index) ) {
                solutions_count += count_solutions(sudoku, square_index + 1);

                if (solutions_count >= 2)
                    break;
            }
        }
        sudoku[y * 9 + x] = 0;
    }

    else {
        solutions_count += count_solutions(sudoku, square_index + 1);
    }

    return solutions_count;
}


int solve_sudoku_randomly(int sudoku[81], int square_index) {

    if (square_index == 81) {
        //printf("  SOLVED!\n");
        return 0;
    }

    int x = square_index % 9;
    int y = square_index / 9;

    if (sudoku[y * 9 + x] == 0) {

        int order[9];
        for (int i = 0; i < 9; i++) {
            order[i] = i + 1;
        }

        for (int i = 8; i > 0; i--) {
            int j = rand() % (i + 1);
            int temp = order[i];
            order[i] = order[j];
            order[j] = temp;
        }

        for (int i = 0; i < 9; i++) {

            sudoku[y * 9 + x] = order[i];

            if (!validate( sudoku, square_index) ) {
                if (!solve_sudoku_randomly(sudoku, square_index + 1))
                    return 0;
            }
        }
        sudoku[y * 9 + x] = 0;
    }
    else {
        if (!solve_sudoku_randomly(sudoku, square_index + 1))
            return 0;
    }
    return 1;
}


int validate(int sudoku[], int square_index) {

    int x = square_index % 9;
    int y = square_index / 9;

    int compare_value = sudoku[square_index];

    for (int i = 0; i < 9; i++) {
        if (sudoku[y * 9 + i] == compare_value && i != x) {
            return 1;
        }

        if (sudoku[i * 9 + x] == compare_value && i != y){
            return 1;
        }
    }

    int bx = (x / 3) * 3;
    int by = (y / 3) * 3;

    for (int ix = bx; ix < bx + 3; ix++) {
        for (int iy = by; iy < by + 3; iy++) {
            if (sudoku[iy * 9 + ix] == compare_value && (ix != x && iy != y)) {
                return 1;
            }
        }
    }
    return 0;
}


void print_sudoku(int sudoku[81]) {

    printf("\n");
    for (int i = 0; i <9; i++) {
        if (i % 3 == 0) {
            printf("\n");
        }
        for (int j = 0; j <9; j++) {
            if (j % 3 == 0)
                printf("  ");
            printf(" %d ", sudoku[i * 9 + j]);

        }
        printf("\n");
    }
}


void print_sudoku_oneline(int sudoku[81]) {

    for (int i = 0; i < 81; i++) {
        printf("%d",sudoku[i]);
    }
}


int *create_sudoku(int difficulty){

    int order[81];
    for (int i = 0; i < 81; i++) {
        order[i] = i;
    }

    for (int i = 80; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = order[i];
        order[i] = order[j];
        order[j] = temp;
    }

    int failed_holes = 0;
    int hole_index;
    int* sudoku = malloc(81 * sizeof(int));

    if (sudoku == NULL) {
        return NULL;
    }

    for (int i = 0; i < 81; i++) {
        sudoku[i] = 0;
    }

    solve_sudoku_randomly(sudoku,0);

    for (int i = 0; i < difficulty; i++) {
        hole_index = order[i];


        int removed_value = sudoku[hole_index];
        sudoku[hole_index] = 0;

        if (count_solutions(sudoku,0) > 1) {
            failed_holes++;
            sudoku[hole_index] = removed_value;
            /*if (difficulty < 80)
                difficulty++;*/
        }
    }
    holes_created = difficulty - failed_holes;
    return sudoku;
}
