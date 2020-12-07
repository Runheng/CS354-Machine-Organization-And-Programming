////////////////////////////////////////////////////////////////////////////////
// Main File:        check_queens.c
// This File:        check_queens.c
// Other Files:      none
// Semester:         CS 354 SPRING 2019
//           
// Author:           Runheng Lei
// Email:            rlei5@wisc.edu
// CS Login:         runheng
//           
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//           
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//           
// Online sources:   avoid web searches to solve your problems, but if you do
//                   search, be sure to include Web URLs and description of 
//                   of any information you find.
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h> 
#include <stdlib.h>  
#include <string.h>   
 
char *COMMA = ",";
  
/* COMPLETED:
 * Retrieves from the first line of the input file,
 * the number of rows and columns for the board.
 * 
 * fp: file pointer for input file
 * rows: pointer to number of rows
 * cols: pointer to number of columns
 */
void get_dimensions(FILE *fp, int *rows, int *cols) {  
        char *line = NULL;  
        size_t len = 0;  
        // check if error exists
      	if (getline(&line, &len, fp) == -1) {  
                printf("Error in reading the file\n");  
                exit(1);  
        }  
        // split the line when detecting comma
        char *token = NULL; 
        token = strtok(line, COMMA);
        *rows = atoi(token); 
           
        token = strtok(NULL, COMMA); 
        *cols = atoi(token);
}      


/* 
 * Returns 1 if and only if there exists at least one pair
 * of queens that can attack each other.
 * Otherwise returns 0.
 * 
 * board: heap allocated 2D board
 * rows: number of rows
 * cols: number of columns
 */
int check_queens(int **board, int rows, int cols) {  
    // use nested loops to search for the first queen on the board
    for(int r = 0; r < rows; r++){
        for(int c = 0; c < cols; c++){
            // when a queen is found
            if(*(*(board+r)+c) == 1){
                // case 1: check if another queen is in the same row
                for(int i = c+1; i < cols; i ++){
                    if(*(*(board+r)+i) == 1){
                        return 1;
                    }
                }
                // case 2: check if another queen is in the same col
                for(int j = r+1; j < rows; j++) {
                    if (*(*(board + j) + c) == 1) {
                        return 1;
                    }
                }
                int x = r;
                int y = c;
                // check if another queen is in the same top-left to bottom-right diagonal
                while((x+1) <rows && (y+1) <cols){
                    if(*(*(board+x+1)+y+1)==1){
                        return 1;
                    }
                    x++;
                    y++;
                }
                // check if another queen is in the same top-right to bottom-left diagonal
                x= r;
                y = c;
                while((x+1) < rows && (y-1)>=0){
                    if(*(*(board+x+1)+y-1) == 1){
                        return 1;
                    }
                    x++;
                    y--;
                }
            }
        }
    }

        return 0;
}     


/* PARTIALLY COMPLETED:
 * This program prints true if the input file has any pair
 * of queens that can attack each other, and false otherwise
 * 
 * argc: CLA count
 * argv: CLA value 
 */
int main(int argc, char *argv[]) {        

        // Check if number of command-line arguments is correct.
      if(argc != 2){
            printf("Usage: ./check_queens <input_filename>\n");
            exit(1);
        }   

        //Open the file and check if it opened successfully.
        FILE *fp = fopen(*(argv + 1), "r");
        if (fp == NULL) {
                printf("Cannot open file for reading\n");
                exit(1);
        }


        //Declare local variables.
        int rows, cols;


        // Call get_dimensions to retrieve the board dimensions.
        get_dimensions(fp, &rows, &cols);

        // Dynamically allocate a 2D array of dimensions retrieved above.
        int **board;
        board = malloc(sizeof(int*)*rows);
        for(int i = 0; i < rows; i++){
            *(board+i) = malloc(sizeof(int)*cols);
        }
        
	    // check if error exists with memory allocation
        if(board == NULL){
            printf("Error in memory allocation!");
            exit(1);
        }         

        //Read the file line by line.
        //Tokenize each line wrt comma to store the values in your 2D array.
        char *line = NULL;
        size_t len = 0;
        char *token = NULL;
        for (int i = 0; i < rows; i++) {

		// check if error exists when tring to read the line
                if (getline(&line, &len, fp) == -1) {
                        printf("Error while reading the file\n");
                        exit(1);        
                }

                token = strtok(line, COMMA);
                for (int j = 0; j < cols; j++) {
                        // initialize the  2D array.
                        *(*(board+i)+j) = atoi(token);
                        token = strtok(NULL, COMMA);    
                }
        }

        // Call the function check_queens and print the appropriate
        //output depending on the function's return value.
        int result = check_queens(board, rows, cols);
        if(result == 1){
            printf("true\n");
        }
        else{
            printf("false\n");
        }
        
        // Free all dynamically allocated memory.
        for(int i = 0; i < rows; i ++){
            free(*(board+i));
        }
        free(board);

        //Close the file.
        if (fclose(fp) != 0) {
                printf("Error while closing the file\n");
                exit(1);        
        }

        return 0;
}      
