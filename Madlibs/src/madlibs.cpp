/* Program madlibs.cpp by Luke Minsuk Kim.
 * Fall 2016-2017 CS106X
 * Techniques involved: string manipulation,file I/O,console I/O
 * References: CS106X lecture code [  https://web.stanford.edu/class/cs106x/lectures/09-30/readfile.cpp  ]
 * The program works as expected for all .txt files under the file res
 */

/******** includes ********/
#include "console.h" // popping up stanford graphical console upon running the program.
#include <iostream> // for cin,cout i.e. console I/O
#include <fstream> // for file I/O
#include "filelib.h" // for checking if file exists.
#include "simpio.h" // Stanford library for input/output
#include <cstdlib> // general utilities library
#include <string> // for string manipultion
#include <sstream> // for string stream
using namespace std;

/******** function prototypes ********/
void printOpeningPrompt(); // prints the opening prompt of the program.
string getFileName();  // prompts user for valid file name, then returns file name.
void Process(string &filename); // solve the problem i.e. identifies placeholder->gets user input->build up an answer that we print in the end.
void processingLines(string &filename,string &line,string &userMadLibs); //For each line checks if the word in each line is placeholder and perform appropriate operations.
void edgeCase(string &s,string &userMadLibs); // for edge cases where we have placeholders adjacent to each other:specifically for torture1.txt. So for example strings such as <noun><noun>
void firstCase(string &word,string &userMadLibs); // when we have found a single word in a placeholder
void secondCase(string &word, bool &addon, string &trailing); // when we have found a segment of the word in a place holder i.e. if we have <x y z> we found <x
void thirdCase(string &word,bool &addon,string &trailing,string &userMadLibs); // when we have found a segment of the word in a place holder i.e. if we have <x y z> we found y or z>
void fourthCase(string &word,string &userMadLibs); // all case other than edge,first,second and third. We simply append string to the answer we are building.
string refine(string &s); // once we extract the <...> string, it gets rid of the the brackets <,>
string chkFront(string &s); // checks if there are any trailing characters in front of <... e.g.: "
string chkBack(string &s); // checks if there are any trailing characters behind ...> e.g.: .,?!"
void printingFormat(string &s); // prints statement for user to fill in the brackets.


/******** main function ********/
int main() {
    printOpeningPrompt();
    string filename = getFileName();
    Process(filename);
    return 0;
}

/******** definition of function prototypes ********/
void printOpeningPrompt(){
    cout << "Welcome to CS 106X Mad Libs!" << endl;
    cout << "I will ask you to provide various words" << endl;
    cout << "and phrases to fill in a story." << endl;
    cout << "At the end, I will display your story to you." << endl;
    cout << endl;
}

string getFileName(){
    string filename;
    while (true){
        filename = getLine("Mad Lib input file?");
        if (fileExists(filename)){
            break;
        }else{
            cout << "Unable to open that file. Try again." << endl;
        }
    }
    return filename;
}

void Process(string &filename){
    cout << endl;
    string line; // stores each line that we get from the file
    string userMadLibs; // stores answer that we print
    processingLines(filename,line,userMadLibs);
    cout << endl;
    cout << "Your Mad Lib story:" << endl;
    cout << userMadLibs;
    return;
}

void processingLines(string &filename,string &line,string &userMadLibs){
    ifstream fin(filename);
    while (getline(fin,line)){
        istringstream tokens(line);
        string word; // words that appears per line
        string add; // what we add to the userMadLibs
        string trailing; // for placeholders of form <x y ... z>
        bool addon = false; // for placeholders of form <x y ... z>
        while (tokens >> word){
            string cmp = userMadLibs;
            edgeCase(word,userMadLibs); // check edge case
            if (cmp != userMadLibs) continue;
            if (word.find("<") != string::npos && word.find(">") != string::npos){
                // we find a single word contained in the placeholder
                firstCase(word,userMadLibs);
            }else if (word.find("<") != string::npos && word.find(">") == string::npos){
                // we found an opening portion of placeholder but it is incomplete,
                // so we can't add it to the answer we are building yet.
                secondCase(word,addon,trailing);
            }else if (addon){
                // we found a closing segment of placeholder so we can add it to our answer
                // or we found a portion of the placeholder but it is not yet complete so we can't add it to our answer.
                thirdCase(word,addon,trailing,userMadLibs);
            }else{ // all else
                fourthCase(word,userMadLibs);
            }
        }
        if (trailing.size() > 0) userMadLibs += trailing; // for words that are half placeholders even until we reach the end of the line.
        userMadLibs += "\n"; // add new line
    }
    return;
}


void edgeCase(string &s, string &userMadLibs){
    int opencnt = 0,closecnt = 0;
    for (int i = 0; i < (int)s.size(); i++){
        if (s[i] == '<') ++opencnt;
        else if (s[i] == '>') ++closecnt;
    }
    if (opencnt > 1 && opencnt == closecnt){ // we can use this conditional because
        // we know that there's at least one character between <> and that there are no <> between <>
        // the operation below may also be performed using the stack data structure.
        string ask;
        bool flag = false;
        for (int i = 0; i < (int)s.size(); i++){
            if (flag) ask += s[i];
            if (s[i] == '<'){
                ask += s[i];
                flag = true;
            }else if (s[i] == '>'){
                string str = refine(ask);
                printingFormat(str);
                string add = getLine();
                userMadLibs += add; // don't need to attach a space after this because placeholders are adjacent.
                ask = "";
                flag = false;
            }
        }
    }
    return;
}

void firstCase(string &word,string &userMadLibs){
    string punctuation_front = chkFront(word); // for cases like "<...
    string punctuation_back = chkBack(word); // for cases like >" or >, or >, etc.
    string str = refine(word); // getting rid of placeholders and other expressions for asking the user
    printingFormat(str); // deciding whether to put a or an in front of the word
    string add = getLine(); // get user input
    // construct answer
    userMadLibs += punctuation_front;
    userMadLibs += add;
    userMadLibs += punctuation_back;
    userMadLibs += " ";
    return;
}

void secondCase(string &word,bool &addon,string &trailing){
    addon = true;
    trailing += word;
    trailing += " ";
    return;
}

void thirdCase(string &word,bool &addon,string &trailing,string &userMadLibs){
    if (word.find(">") != string::npos){
        trailing += word;
        string punctuation_front = chkFront(trailing);
        string punctuation_back = chkBack(trailing);
        string str = refine(trailing);
        printingFormat(str);
        string add = getLine();
        userMadLibs += punctuation_front;
        userMadLibs += add;
        userMadLibs += punctuation_back;
        userMadLibs += " ";
        trailing = "";
        addon = false;
    }else{
        trailing += word;
        trailing += " ";
    }
    return;
}

void fourthCase(string &word,string &userMadLibs){
    // it is not a placeholder so simply add it to the answer we are building.
    userMadLibs += word;
    userMadLibs += " ";
    return;
}

string refine(string &s){
    int idx;
    for (int i = 0; i < (int)s.size(); i++){
        if (s[i] == '<'){
            idx = i;
            break;
        }
    }
    string t = s.substr(idx+1);
    for (int i = 0; i < (int)t.size(); i++){
        if (t[i] == '>'){
            idx = i;
            break;
        }
    }
    t = t.substr(0,idx);
    return t;
}


string chkFront(string &s){
    int idx;
    for (int i = 0; i < (int)s.size(); i++){
        if (s[i] == '<'){
            idx = i;
            break;
        }
    }
    return s.substr(0,idx);
}


string chkBack(string &s){
    int idx;
    for (int i = 0; i < (int)s.size(); i++){
        if (s[i] == '>'){
            idx = i;
            break;
        }
    }
    return s.substr(idx+1);
}

void printingFormat(string &s){
    // we identify the position when an alphabet first appears.
    // this is to avoid printing wrong output for cases such as < adjective > as in torture2.txt
    int idx;
    for (int i = 0; i < (int)s.size(); i++){
        if ((s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z')){
            idx = i;
            break;
        }
    }
    // check if the first letter that appears is a vowel or a consonant.
    if (s[idx] == 'a' || s[idx] == 'e' || s[idx] == 'i' || s[idx] == 'o' || s[idx] == 'u' ||
        s[idx] == 'A' || s[idx] == 'E' || s[idx] == 'I' || s[idx] == 'O' || s[idx] == 'U'){
        cout << "Please type an " << s <<  ": ";
    }else{
        cout << "Please type a " << s << ": ";
    }
    return;
}
