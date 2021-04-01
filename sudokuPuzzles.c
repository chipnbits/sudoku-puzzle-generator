/*********************************************************************************************************************/
/* Sudoku puzzle solver by Simon Ghyselincks
 * sghyselincks@gmail.com
 * Jan 3rd 2021
 *
 *  Creates a pseudo-random solvable Sudoku puzzle with a unique solution and prints it.
 *  When prompted by the user it will then display the unique solution.
 *
 *  The user can define the board as SIZE 4, 9, or 16  (size 25 is too complex for this program to compute)
 *  When using size 16 boards, limit the empty cells MAX_EMPTY to 130 to avoid overly-long computation
 *  
 *  There is a Sudoku puzzle solver built into the program that can also be used to solve externally generated cases
 *
 *  The Sudoku solution board is generated using a recursive backtracking algorithm that substitutes a random integer
 *  into an empty cell and checks if it will lead to a solution.
 *
 *  The Sudoku puzzle is generated from a solution board by emptying random position cells until
 *  there are no longer any cells that can be emptied that would still lead to a unique solution
 *
 *  The number of solutions on a sudoku board is checked through a recursive backtracking algorithm that
 *  takes the sum of all complete boards that can be reached from the current board state
 *
 */

#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <Windows.h>

#define TRUE 1
#define FALSE 0
#define EMPTY 0 // Placeholder for empty Sudoku board positions

#define SIZE 9  //The order of magnitude of the board  Always a squared value.. 2^2, 3^2, 4^2

/* This value is needed for a board size of 16, specifies how many empty cells to leave in a puzzle
 * Try values of 130-135 combined with 16x16 Sudoku Boards
 */
#define MAX_EMPTY 81 // the maximum number of empty cells for a puzzle generated

// Global backtrack counter for tracking solution branches
int backtrackCount = 0;

/* Function prototypes */

/* Puzzle Generation */
int generateBoard(int board[][SIZE]);
int randomFillBoard(int board[][SIZE]);
int generatePuzzle(int board[][SIZE], int solution[][SIZE]);

/* Functions manipulating Sudoku boards */
void duplicateBoard(int read[][SIZE], int write[][SIZE]);
void printBoard(int board[][SIZE]);
int solveBoard(int board[][SIZE], int solution[][SIZE]);

/* Funtions operating on Sudoku cell values */
int nextEmpty(int board[][SIZE], int* add_xVal, int* add_yVal);
int getValidIntegers(int board[][SIZE], int xPos, int yPos, int validIntegers[SIZE]);
void permittedValue(int board[][SIZE], int xPos, int yPos, int permitted[SIZE+1]);

/* A list shuffling function */
void shuffleValues(int list[SIZE], int listSize);


int main(void) {
	// A test case provided for debugging solving methods
	//int testCase[SIZE][SIZE] = {0,2,0,0,0,0,0,0,0,
	//							0,0,0,6,0,0,0,0,3,
	//							0,7,4,0,8,0,0,0,0,
	//							0,0,0,0,0,3,0,0,2,
	//							0,8,0,0,4,0,0,1,0,
	//							6,0,0,5,0,0,0,0,0,
	//							0,0,0,0,1,0,7,8,0,
	//							5,0,0,0,0,9,0,0,0,
	//							0,0,0,0,0,0,0,4,0};	

	/* seed the random number generator with the current time */
	srand(time(NULL));

	/* Create arrays to hold puzzle and solution */
	int solution[SIZE][SIZE] = { 0 };   // A completed Sudoku problem
	int puzzle[SIZE][SIZE] = { 0 };     // A Sudoku puzzle

	/* user input functionality*/
	char pressEnter = '\n';
	

	/* First we generate a random and complete solution board */
	if (!generateBoard(puzzle))
		printf("Warning, error generating a Sudoku puzzle.\n");

	/* Make a Sudoku puzzle from a complete board by removing cells until
	 * the further removal of any cell on the board would result in a 
	 * non-unique solution.
	 */ 	
	int emptyCells = generatePuzzle(puzzle, solution);
	   
	/* Print the problem board */
	printBoard(puzzle);
	printf("\n\n");

	/* Wait for user input before displaying a solution */
	printf("There are %d cells already filled in on this Sudoku board.\n", SIZE*SIZE - emptyCells);
	printf("Press ENTER to display the solution.\n");
    scanf("%c", &pressEnter);

	/* Display the solution to the puzzle*/
	printBoard(solution);
	printf("\n\n");

	return 0;
}

/* Generate a blank Sudoku board and then fill it with randomized 
 *  legal values, return 1 if successful
 */

int generateBoard(int board[][SIZE]) {

	//initialize the board with 0s
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			board[i][j] = 0;
		}
	}

	randomFillBoard(board); // Fill the empty board with random values

	return 1; // return 1 if successful
}

/* Fill a Sudoku board with pseudo-random values using recursive backtracking.
 * All filled cells must be legal within the rules of Sudoku.
 *
 * Input: an empty or partially filled Sudoku board
 * Output: Return TRUE if board has been filled, 
 *         FALSE if the board is unsolvable (no legal moves)
 */

int randomFillBoard(int board[][SIZE]) {
	int xPos = 0; // Column index of a Sudoku cell
	int yPos = 0; // Row index of a Sudoku cell
	
	int validIntegers[SIZE] = { 0 }; // List of valid integers for a cell in randomized order 
	int listSize;  // The number of items in a list of valid integers

	int solved = FALSE; // Indicates if the board has been completely filled with legal values

	if ( !(nextEmpty(board, &xPos, &yPos)) ){ // Find the next empty cell on the board,
											 // If there are no more empty cells
		solved = TRUE;                       // Then the board has been completed (solved) 

	}else{ // The board isn't solved yet so continue solving

		// Next, get a list of permitted values for the empty cell, if there are no
		// Permitted values, then the board is unsolvable, return FALSE
		listSize = getValidIntegers(board, xPos, yPos, validIntegers);
		if (listSize <= 0) {
			// No valid integers, the board is unsolvable
			solved = FALSE;
		}
		else {  // There are valid integers for substitution, try substituting each one until solution found
			
			// Shuffle the list of valid integers to ensure randomness
			shuffleValues(validIntegers, listSize);

			/* Try putting each legal value int the list into the empty Sudoku cell 
			 * until a solution is found (while !solved), use recursion with backtracking here
			 */
			for (int i = 0; i < listSize && !solved; i++) { 

				/* Replace the empty position with a shuffled valid integer */
				board[yPos][xPos] = validIntegers[i];
				
				/* This is the recursion step to make sure the substitution leads to a solution*/
				solved = randomFillBoard(board);  

				/* if there is no solution found for this branch, then it is important to replace the Sudoku cell
				 * with an EMPTY value, since the array is shared across all iterations of the function
				 */
				if (!solved) {
					board[yPos][xPos] = EMPTY;
				}				
			}
		}
	}
	return solved;
}

/*  Takes a completed and legal Sudoku board and removes random cells from it until there 
 *  is only one unique solution to the puzzle. Uses a list of all the cells on the board, 
 *  numbered from 0 - SIZE^2 - 1 -- e.g. for a 9x9 board there are 81 positions numbered from 0 - 80
 *   
 *    i. e.  0  1  2  |  3  4  5  |  6  7  8
 *           9  10 11 |  12 13 14 |  15 16 17
 *           . . . . . . . . . . . . . . . . 
 *           72 73 74 |  75 76 77 |  78 79 80
 *
 *  Removes numbers one at a time until the next removal would result in a non unique-solution puzzle
 *  
 *  Returns the value of the number of cells that were emptied from the solution to form the puzzle
 */

int generatePuzzle(int board[][SIZE], int solution[][SIZE]) {

	int cellNumber;  // A number representing a Sudoku cell from 0 - SIZE^2
	int listOfCells[SIZE*SIZE] = { 0 };  // A list of numbered cell values

	int xPos; // The x position of a Sudoku cell
	int yPos; // The y position of a Sudoku cell
	int cellValue; // The value read from a Sudoku cell

	int removedCount = 0;  // Count how many cells have been successfully emptied from the full board
	int solutions = 0;  // Tracks the solutions found by removing the number

	// Initialize the list
	for (int i = 0; i < SIZE*SIZE; i++) {
		listOfCells[i] = i;
	}

	// Shuffle the list to randomize order
	shuffleValues(listOfCells, SIZE*SIZE);

	// Empty the cell values in the list one by one until a unique-solution puzzle has been made.
	int index = 0;
	while (index < SIZE*SIZE &&  removedCount < MAX_EMPTY) {
		
		cellNumber = listOfCells[index]; // Draw the next randomized cell postion from the list

		// Convert the cellNumber to an {x,y} postion on the board
		yPos = (cellNumber / SIZE);  //Integer division conveniently yields the y position
		xPos = (cellNumber % SIZE);  //And the remainder is the x postion

		// Try removing the cell to see if removing it prevents a unique solution, while holding removed value in memory
		cellValue = board[yPos][xPos];
		board[yPos][xPos] = EMPTY;

		// Determine number of potential solutions after emptying the most recent cell
		solutions = solveBoard(board, solution);

		// If there is more than one solution now, the cell can't be removed without violating
		// creating a unique solution.  Replace the cell's value and continue the loop
		if (solutions > 1) {      
			board[yPos][xPos] = cellValue;							
		}
		else {
			removedCount++;
		}
		index++;		
	}
	return removedCount;
}


/* Duplicates a Sudoku board value for value reading from read[][], writing to write[][]*/
void duplicateBoard(int read[][SIZE], int write[][SIZE]) {

	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			write[i][j] = read[i][j];
		}
	}
}

/* Displays a formatted rendering of the Sudoku board for viewing in the console
 * Formatting lines indicate the sub-square boundaries
 */
void printBoard(int board[][SIZE]) {

	// Calculate the length of a subsquare (number of positions)
	int subSquareLength = (int)sqrt(SIZE);

	for (int i = 0; i < SIZE; i++) {

		//This section inserts a horizontal line of suitable length to visually divide the subsquares
		if (i % subSquareLength == 0 && i > 0) {    // Determine if a horizontal line should be inserted
			for (int space = 0; space < SIZE; space++) {  //Fill it with dashes
				printf("---");
			}
			//Add extra dashes to account for the vertical sub-square lines inserted
			for (int space = 0; space < subSquareLength - 1; space++) {  
				printf("---");
			}
			printf("\n"); // Line break to start new row
		}

		// The numerical values are filled in with vertical line breaks for each subsquare division
		for (int j = 0; j < SIZE; j++) {
			if (j % subSquareLength == 0 && j > 0) {  // Determine if vertical line needed
				printf("  |");
			}
			printf("%3d", board[i][j]);
		}

		printf("\n");  // Line break to start new row
	}
}

/* Given an array of Sudoku values, this function will return total number of solutions, 0 if a solution has not been found
 * It is recursive with backtracking.  Finds the index of the next empty value in the array and
 * tries all possible combinations with it.  Returns 0 if no solution exists.
 */
int solveBoard(int board[][SIZE], int solution[][SIZE]) {

	// Track the total solutions reachable from this node
	int totalSolutions = 0;

	/* The {x, y} coordinates of a cell in the Sudoku puzzle */
	int xPos;
	int yPos;
	int validIntegers[SIZE] = { 0 }; // A list of which integers are valid options to substitute into an empty Sudoku cell
	int listSize = 0; // The number of legal integers that can be used to fill a cell

	if (nextEmpty(board, &xPos, &yPos)) {  //Find the next empty Sudoku cell, if there is one then continue

		listSize = getValidIntegers(board, xPos, yPos, validIntegers);
		if (listSize > 0) { //Check if the empty position has any legal integer values

			for (int i = 0; i < listSize; i++) {  // Loop to check all the permitted values for potential solutions				

				board[yPos][xPos] = validIntegers[i];  // Fill the cell with a valid integer from the list

				// Recursive step here, checks for all possible solutions descending from the the cell
				totalSolutions += solveBoard(board, solution); // Add the solutions from each branch to the total	

				board[yPos][xPos] = EMPTY; // Fill the cell with the previous EMPTY value

				backtrackCount++;          // track how many nodes visited
			}
		}

		else {  // No legal values to complete the empty cell, zero solutions from this terminating branch
			totalSolutions = 0;
		}
	}
	else { // No more empty values, the board has been solved
		if (totalSolutions == 0) {  //If this is the first solution that has been found (no other solved cases) save to solution
			duplicateBoard(board, solution);  // save the solution board
		}
		totalSolutions++; // Add this terminating branch as a valid solution
	}
	return totalSolutions;
}

/* Find the x and y coordinate values of the next available empty position in the Sudoku board.
 * if there are no more empty positions, then the function returns FALSE (puzzle has been solved).
 * otherwise the coordinates are returned by array { xValue, yValue }
 */
int nextEmpty(int board[][SIZE], int* add_xVal, int* add_yVal) {
	int found = FALSE;  //Tracks if an empty value has been found yet
	int xVal = 0;
	int yVal = 0;

	// Scan row by row, when coordinates of EMPTY are found it will freeze the x-y vals by exiting the loop
	while (!found && yVal < SIZE) {

		while (!found && xVal < SIZE) {
			if (board[yVal][xVal] == EMPTY) {
				found = TRUE;
			}
			else {
				xVal++;  // Try the next value if previous wasn't EMPTY
			}
		}
		
		if (!found) {  //If an empty value wasn't found in the last row of values
			yVal++;    //Try the next row and
			xVal = 0;  //Reset row position
		}
	}

	// Pass by pointer the values to the calling function
	if (found) {
		*add_xVal = xVal;
		*add_yVal = yVal;
	}
	return found;
}

/* Populate a list of all possible legal integers to fill a Sudoku cell based on the surrounding values of the cell
 * Inputs: - board - a Sudoku board
 *		   - xPos, yPos - The coordinates of the Sudoku cell to be inspected
 *         - validIntegers - The array to store the list of values in
 *
 * Output: - (implicit) The values are stored in validIntegers which can be accessed by the calling function
 *	       - intIndex - The total number of valid integer options found for the Sudoku cell, returns 0 if no legal moves
 */		   

int getValidIntegers(int board[][SIZE], int xPos, int yPos, int validIntegers[SIZE]) {
	int permitted[SIZE + 1] = { 0 }; // Array to store the permission values found with permittedValue()

	/* Test every possible integer for the cell if it is a legal move or not
	 * permitted[] is populated with TRUE/FALSE values for each index matching 
	 * a possible integer to fill a cell, indicating if the integer is a legal move.
	 */
	// Retrieve the list of permissions for the Sudoku cell
	permittedValue(board, xPos, yPos, permitted);

	int permitIndex = 1;  // Start at index 1 of the permittedValues array
	int intIndex = 0;     // Start at 0 index for the array of integer values
	do {
		if (permitted[permitIndex])
		{ // Since the permission index represents the integer, add that index to the list
			validIntegers[intIndex] = permitIndex;
			intIndex++;
		}
		permitIndex++;
	} while (permitIndex <= SIZE);  // Scan all values in the permissions array	 

	return intIndex; // The total number of valid integers is equal the final value of intIndex
}

/* Inspects a cell on a Sudoku board and verifies which integer values might be permitted in that Sudoku cell
 * Checks the column, row, and square of that Sudoku cell to remove invalid integer options 
 * Input : board - a solved or unsolved Sudoku board
 *         coords - the coordinates of the Sudoku cell to be inspected
 *         permitted - an array of TRUE or FALSE values that will record and return results
					   The array represents valid integer options as yielding a TRUE value at
					   the index of that integer.
 *
 * Output : (implicit) permitted[] -  records which integers can be legally substituted into a board position
 *          the integer of the array index position is the integer value in question, TRUE means it is permitted
 */         

void permittedValue(int board[][SIZE], int xPos, int yPos, int permitted[SIZE+1]) {
	int scannedVal;
	int valid = FALSE;  // Value indicating if there are any permitted values

	// Values to bound the Sudoku sub-square that the coordinate belongs to
	int xMin, xMax, yMin, yMax;
	// The size of a sub-square
	int squareSize = sqrt(SIZE);

	// Configure the permitted[] array to read TRUE for all integers (not including zero) to start
	for (int i = 1; i < SIZE + 1; i++) {
		permitted[i] = TRUE;
	}
	permitted[EMPTY] = FALSE; // The 0 EMPTY cell value is not a legal option

	// Scan the row of the position to check value
	for (int xval = 0; xval < SIZE; xval++) {
		// Find the integer value of one of the completed numbers in the row
		scannedVal = board [yPos] [xval];
		// Exclude that number form the permitted values
		permitted[scannedVal] = FALSE;
	}

	// Scan the column of the position to eliminate values not permitted
	for (int yval = 0; yval < SIZE; yval++) {
		// Find the integer value of one of the completed numbers in the column
		scannedVal = board [yval] [xPos];
		// Exclude that number form the permitted values
		permitted[scannedVal] = FALSE;
	}

	// Define the sub-square that the coordinate postion occupies
	// sub-square position is found by integer division, multiplication converts to the proper board position
	xMin = (xPos / squareSize) * squareSize;  
	xMax = xMin + squareSize - 1;
	yMin = (yPos / squareSize) * squareSize;
	yMax = yMin + squareSize - 1;

	//Find any already used integers from the Sudoku sub-square
	for (int i = yMin; i <= yMax; i++) {
		for (int j = xMin; j <= xMax; j++) {
			scannedVal = board[i][j];
			permitted[scannedVal] = FALSE;
		}
	}
}

/* Take an ordered list of integers and shuffle the values into a randomly ordered list 
 * A list is defined as a string of integers of size listSize, with the last item of the
 * placed at index [listSize - 1] since the list items start at index 0.
 *
 */
void shuffleValues(int list[SIZE], int listSize) {

	int randomIndex; // A randomly selected index 
	int completedIndex = 0; // The index of items that have been successfully shuffled
	int listItem;  // The current list item selected in the shuffling process

	do {
		//Select a random index that covers the first listSize number of items in the array
		randomIndex = rand() % listSize;

		// Use the random index to draw one item from the list
		listItem = list[randomIndex];

		/* Scratch the value from the list
		 * Start at the randomIndex and overwrite each value in the list with the
		 * next value in the list
		 */
		for (int i = randomIndex; i < listSize; i++) {
			list[i] = list[i + 1];
		}

		// Reduce the unshuffled listSize by one since an item is removed
		listSize--;

		// Place the removed list item back in the array in the newly vacated spot
		list[listSize] = listItem;

		// Increment completedIndex, an item was added to the shuffled deck
		completedIndex++;

	} while (listSize > 0); //Continue until the entire list is shuffled
}

