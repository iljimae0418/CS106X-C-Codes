/* ngrams.cpp by Luke Minsuk Kim
 * Fall 2016-2017
 * We build a map of N-Grams and generate random text.
 * Works as expected for all given test files.
 */

/**** includes ****/
#include <iostream> // for console I/O
#include <fstream> // for file I/O
#include <cstdlib> // general utilities library
#include "console.h" // for using console
#include "filelib.h" // for promptUserForFile()
#include "simpio.h" // for getInteger()
#include "random.h" // for randomInteger()
#include "map.h" // for using Stanford map library
#include "vector.h" // for using Stanford vector library
#include <string> // for manipulating string objects.
#include <sstream> // in case we need string stream.
using namespace std;

/**** function prototypes ****/
void openingPrompt();
void getCollection(const string &filename,Vector<string> &collection);
int getN(const Vector<string> &collection);
void buildNGram(Map< string,Vector<string> > &map,const Vector<string> &collection,const int &N);
int getNumWords(const int &N);
void generateRandomText(const Map< string,Vector<string> > &map,const int &N);

/**** main function ****/
int main() {
    openingPrompt();
    string filename = promptUserForFile("Input file name: ","Unable to open that file. Try again.");
    Vector<string> collection;
    getCollection(filename,collection);
    int N = getN(collection);
    if (N == -1){
        // If getN() returns -1, the value of N given is greater than the total number of words
        // in the file. We quit the program.
        return 0;
    }
    cout << endl;
    Map< string,Vector<string> > map;
    buildNGram(map,collection,N);
    generateRandomText(map,N);
    return 0;
}

/**** function definitions ****/

// prints the opening prompt of the program
void openingPrompt(){
    cout << "Welcome to CS 106X Random Writer ('N-Grams')!" << endl;
    cout << "This program generates random text based on a document." << endl;
    cout << "Give me an input file and an 'N' value for groups of" << endl;
    cout << "words, and I will generate random text for you." << endl;
    cout << endl;
}

// stores all the words in the file in a vector of strings.
void getCollection(const string &filename,Vector<string> &collection){
    ifstream fin(filename);
    string word;
    while (fin >> word){
        collection.add(word);
    }
}

// prompts the user for the value of N and handles exceptions.
int getN(const Vector<string> &collection){
    int N;
    while (true){
        N = getInteger("Value of N: ");
        if (N < 2){
            cout << "N must be 2 or greater." << endl;
        }else if (N > collection.size()){
            cout << "N is larger than the number of words in the file." << endl;
            cout << "Unable to compute N-grams." << endl;
            cout << "Exiting." << endl;
            return -1;
        }else{
            return N;
        }
    }
    return -1; // will never reach this step though
}

// We build a map of N-Grams i.e. key containing the prefix with N-1 words and value
// containing the list of suffixes that appear.
void buildNGram(Map< string,Vector<string> > &map,const Vector<string> &collection,const int &N){
    for (int i = 0; i < collection.size(); i++){ // i: start index, i.e. the first word in the N-1 prefix.
        string t; // t will contain the N-1 words prefix that will be used as the key.
        t += collection[i];
        t += " ";
        int j,cnt; // use variables j and cnt to track/add the next N-2 words.
        for (j = i+1,cnt = 1; cnt < N-1; j++,cnt++){
            if (j >= collection.size()){
                j = 0;
            }
            t += collection[j];
            if (cnt < N-2){
                t += " ";
            }
        }
        if (j >= collection.size()){
            j = 0;
        }
        map[t].add(collection[j]); // finally adding the Nth word into our list of suffixes for the corresponding prefix key.
    }
}

// prompts the user for the number of words to generate, and also handles exceptions.
int getNumWords(const int &N){
    int numWords;
    while (true){
        numWords = getInteger("# of random words to generate (0 to quit): ");
        if (numWords != 0 && numWords < N){
            cout << "Must be at least " << N << " words." << endl;
            cout << endl;
        }else{
            return numWords;
        }
    }
    return numWords; // will never reach this step though
}

// prompt user for the number of words, then generate that many words
// using the map of NGrams that we already constructed.
void generateRandomText(const Map< string,Vector<string> > &map,const int &N){
    while (true){
        int numWords = getNumWords(N);
        if (numWords == 0){
            cout << "Exiting." << endl;
            break;
        }
        Vector<string> keys = map.keys(); // We declare the vector keys to determine the starting position
        // we generate random text
        cout << "... ";
        int start = randomInteger(0,keys.size()-1);
        cout << keys[start] << " ";
        numWords -= (N-1);
        string current_window = keys[start];
        while (numWords){
            int r = randomInteger(0,map[current_window].size()-1);
            string add = map[current_window][r];
            cout << add << " ";
            int pos = current_window.find(" ");
            current_window = current_window.substr(pos+1);
            current_window += " ";
            current_window += add;
            --numWords;
        }
        cout << "..." << endl;
        cout << endl;
    }
}
