/* life.cpp by Luke Minsuk Kim
 * Fall 2016-2017 CS106X
 * Techniques involved: grid,file I/O,console I/O
 * Brief description of program: Simulates Conway's Game of Life as specified here https://web.stanford.edu/class/cs106x/homework/1-madlibs-life/life-spec.html
 */

#include "console.h" // for printing to console and for animation
#include <iostream> // for console I/O
#include <fstream> // for file I/O
#include "filelib.h" // to check if the file name that the user entered exists.
#include "simpio.h" // for getLine(), getInteger(), getYesOrNo()
#include "lifegui.h" // For GUI to show how the cells change over time.
#include "grid.h" // for manipulating the Grid class
#include <cstdlib> // general utilities library
#include <string> // for C++ string manipulation
#include <sstream> // for string streams
using namespace std;

void openingPrompt(); // prints opening prompt of the program
string getFileName(); // prompts user to enter the file name and returns the file name as a string
Grid<char> setUpGrid(string filename); // initializes grid according to the file name.
void display(Grid<char> &grid); // asks user for animation, tick, or quit and performs an appropriate action.
void animationDisplay(Grid<char> &grid); // generates an animation of the number of frames that the user specified.
void nextGeneration(Grid<char> &grid); // updates the grid for the next generation according to the rulese of Game of Life.
int neighbors(Grid<char> &grid,int rowpos,int colpos); // returns how many neighbors there are
void adjustToroidal(int rowSize,int colSize,int &nrow,int &ncol); // adjusts the row position and column position we are searching to accomodate the toroidal setting of the grid.
void printGrid(Grid<char> &grid); // it prints the grid containing information about filled and empty cells to the grid.
void drawGraphicalWindow(Grid<char> &grid); // it clears the GUI console firt, then draws the grid on the GUI console.

int main() {
    LifeGUI::initialize(); // popping up GUI window.
    openingPrompt();
    string filename = getFileName();
    Grid<char> grid = setUpGrid(filename);  // at this point, grid contains information about the cells.
    display(grid);
    LifeGUI::shutdown();
    return 0;
}

void openingPrompt(){
    cout << "Welcome to the CS 106X Game of Life!" << endl;
    cout << "This program simulates the lifecycle of a bacterial colony." << endl;
    cout << "Cells (X) live and die by the following rules:" << endl;
    cout << "* A cell with 1 or fewer neighbors dies." << endl;
    cout << "* Locations with 2 neighbors remain stable." << endl;
    cout << "* Locations with 3 neighbors will create life." << endl;
    cout << "* A cell with 4 or more neighbors dies." << endl;
    cout << endl;
    return;
}

string getFileName(){
    string filename;
    while (true){
        filename = getLine("Grid input file name?");
        if (fileExists(filename)){
            break;
        }else{
            cout << "Unable to open that file. Try again." << endl;
        }
    }
    return filename;
}

Grid<char> setUpGrid(string filename){
    ifstream fin(filename);
    int row,col;
    fin >> row >> col; // get the number of rows and columns first.
    Grid<char> grid(row,col); // initialize a Grid object with the appropriate number of rows and columns.
    LifeGUI::resize(row,col); // initialize the size of the GUI window for simulation.
    for (int i = 0; i < grid.numRows(); i++){
        string info;
        fin >> info;
        for (int j = 0; j < grid.numCols(); j++){
            grid[i][j] = info[j];
        }
    }
    drawGraphicalWindow(grid);
    return grid;
}

void display(Grid<char> &grid){
    printGrid(grid);
    while (true){
        while (true){
            string command = getLine("a)nimate, t)ick, q)uit?");
            if (command == "a" || command == "A"){ //animation
                animationDisplay(grid);
            }else if (command == "t" || command == "T" || command == ""){ // ticking one generation forward
                nextGeneration(grid);
                printGrid(grid); //printing grid to console.
            }else if (command == "q" || command == "Q"){ // quit
                break;
            }else{ // invalid input
                cout<< "Invalid choice; please try again." << endl;
            }
        }
        if (getYesOrNo("Load another file? (y/n)")){ // returns true if user types 'y'
            string filename = getFileName();
            grid = setUpGrid(filename);
            printGrid(grid);
        }else{
            cout << "Have a nice Life!" << endl;
            break;
        }
    }
    return;
}

void animationDisplay(Grid<char> &grid){
    int frame_no = getInteger("How many frames?");
    for (int i = 0; i < frame_no; i++){
        clearConsole();
        nextGeneration(grid);
        printGrid(grid);
        pause(100);
    }
    return;
}

void nextGeneration(Grid<char> &grid){
    Grid<char> newGrid(grid.numRows(),grid.numCols());
    for (int i = 0; i < grid.numRows(); i++){
        for (int j = 0; j < grid.numCols(); j++){
            int no_of_neighbors = neighbors(grid,i,j);
            if (no_of_neighbors == 0 || no_of_neighbors == 1){ // 0 or 1 neighbor: if cell exists it dies
                newGrid[i][j] = '-';               
            }else if (no_of_neighbors == 2){ // 2 neighbors: stable
                newGrid[i][j] = grid[i][j];
            }else if (no_of_neighbors == 3){ // 3 neighbors: if currently it contains a cell it remains, else a new cell is born.
                newGrid[i][j] = 'X';
            }else if (no_of_neighbors >= 4){ // 4 neighbors: dies of overcrowding
                newGrid[i][j] = '-';
            }
        }
    }
    grid = newGrid;
    drawGraphicalWindow(grid);
    return;
}

int neighbors(Grid<char> &grid,int rowpos,int colpos){
    int cnt = 0;
    int R = grid.numRows(), C = grid.numCols();
    for (int drow = -1; drow <= 1; drow++){
        for (int dcol = -1; dcol <= 1; dcol++){
            if (drow == 0 && dcol == 0) continue;
            int nrow = rowpos + drow;
            int ncol = colpos + dcol;
            adjustToroidal(R,C,nrow,ncol);
            if (grid[nrow][ncol] == 'X'){
                ++cnt;
            }
        }
    }
    return cnt;
}

void adjustToroidal(int rowSize, int colSize, int &nrow, int &ncol){
    if (nrow == -1 && (ncol >= 0 && ncol < colSize)){
        nrow = rowSize-1;
    }else if ((nrow >= 0 && nrow < rowSize) && ncol == -1){
        ncol = colSize-1;
    }else if ((nrow >= 0 && nrow < rowSize) && ncol == colSize){
        ncol = 0;
    }else if (nrow == rowSize && (ncol >= 0 && ncol < colSize)){
        nrow = 0;
    }else if (nrow == -1 && ncol == -1){
        nrow = rowSize-1;
        ncol = colSize-1;
    }else if (nrow == -1 && ncol == colSize){
        nrow = rowSize-1;
        ncol = 0;
    }else if (nrow == rowSize && ncol == -1){
        nrow = 0;
        ncol = colSize-1;
    }else if (nrow == rowSize && ncol == colSize){
        nrow = 0;
        ncol = 0;
    }
    return;
}

void printGrid(Grid<char> &grid){
    for (int i = 0; i < grid.numRows(); i++){
        for (int j = 0; j < grid.numCols(); j++){
            cout << grid[i][j];
        }
        cout << endl;
    }
    return;
}

void drawGraphicalWindow(Grid<char> &grid){
    LifeGUI::clear();
    for (int i = 0; i < grid.numRows(); i++){
        for (int j = 0; j < grid.numCols(); j++){
            if (grid[i][j] == 'X'){
                LifeGUI::fillCell(i,j);
            }
        }
    }
    LifeGUI::repaint();
    return;
}
