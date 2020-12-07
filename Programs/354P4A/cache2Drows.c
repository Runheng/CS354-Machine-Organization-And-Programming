////////////////////////////////////////////////////////////////////////////////
// Main File:        cache1D.c
// This File:        cache2Drows.c
// Other Files:      cache1D.c; cache2Dcols.c
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
// Persons:          none
//
// Online sources:   none
////////////////////////////////////////////////////////////////////////////////

int arr2D[3000][500];

int main(){
    for(int row = 0; row < 3000; row++){
        for(int col = 0; col < 500; col++){
            arr2D[row][col] = row + col;
        }
    }

    return 0;
}