
/* Sudoku puzzle solver by Simon Ghyselincks
 * sghyselincks(at)gmail.com
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
