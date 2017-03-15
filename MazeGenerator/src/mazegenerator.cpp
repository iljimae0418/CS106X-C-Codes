/* mazegenerator.cpp by Luke Minsuk Kim
 * Fall 2016-2017
 * It implements Kruskal's algorithm of
 * 1. randomly picking out a wall from a maze
 * 2. Figuring out which chamber a given location cell is in
 * 3. Figuring out whether two chambers are same
 * 4. Merging two chambers.
 * A 50x50 grid takes 7170ms to be solved, which is not bad.
 */

/**** includes ****/
#include "mazetypes.h" // to use objects of Cell and Wall
#include "mazegui.h" // to utilize instructor-provided graphical user interface.
#include <iostream> // for testing purposes; we are not actually printing anything out from the console.
#include "vector.h" // for Stanford vector library
#include "map.h" // for Stanford map library
#include "set.h" // for Stanford set library
using namespace std;

/**** function prototypes ****/
void init(Map< int,Set<Cell> > &chambers,const Vector<Cell> &allCells);
void Kruskal(Map<int,Set<Cell> > &chambers,const Vector<Cell> &allCells,const Vector<Wall> &allWalls);

/**** generateMaze() function ****/
void generateMaze() {
    Vector<Cell> allCells = MazeGUI::getAllCells(); // returns location cell in the maze as a Vector of Cell objects.
    Vector<Wall> allWalls = MazeGUI::getAllWalls(); // returns all walls between cells. Vector shuffled in random order.
    Map< int,Set<Cell> > chambers;
    init(chambers,allCells);
    Kruskal(chambers,allCells,allWalls);
}

/**** function definitions ****/

// initialize the chamber so that at the start, each chamber contains one unique cell.
void init(Map<int, Set<Cell> > &chambers,const Vector<Cell> &allCells){
    for (int i = 0; i < allCells.size(); i++){
        chambers[i].add(allCells[i]);
    }
}

// implements Kruskal's algorithm described in the spec.
void Kruskal(Map<int, Set<Cell> > &chambers, const Vector<Cell> &allCells, const Vector<Wall> &allWalls){
    for (int i = 0; i < allWalls.size(); i++){
        Wall w = allWalls[i];
        Cell firstOne = w.one, secondOne = w.two;
        bool flag = false;
        for (int j = 0; j < allCells.size(); j++){
            if (chambers[j].contains(firstOne) && chambers[j].contains(secondOne)){
                flag = true;
                break;
            }
        }
        if (flag) continue;
        // else we merge
        int key1 = -1,key2 = -1;
        for (int j = 0; j < allCells.size(); j++){
            if (chambers[j].contains(firstOne)){
                key1 = j;
            }else if (chambers[j].contains(secondOne)){
                key2 = j;
            }
            if (key1 != -1 && key2 != -1){
                break;
            }
        }
        chambers[key1] = chambers[key1]+chambers[key2];
        chambers.remove(key2);
        MazeGUI::removeWall(w);
    }
}
