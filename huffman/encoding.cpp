/* encoding.cpp by Luke Minsuk Kim
 * Fall 2016-2017 CS106X
 * Implements the Huffman Encoding Algorithm
 * Comments above functions contain descriptions of what each of them does
 * The compress and decompress function writes the frequency map as the header and
 * calls other functions appropriately.
 * This program matches result of all output logs.
 * Initially used char data types for some functions (e.g. buildFrequencyTable()) but realised this
 * causes trouble for non text inputs so changed them to in data type.
 */
#include "encoding.h" // already includes map.h
#include "pqueue.h"
#include "vector.h"
#include "filelib.h"
using namespace std;

// Examine a source file's contents and count the number of occurences
// of each character.
Map<int, int> buildFrequencyTable(istream& input) {
    Map<int, int> freqTable;
    int c;
    while (true){ // using .get() because getline() cannot detect '\n'.
        c = input.get();
        if (c == -1){ // reached the end of the file.
            break;
        }
        freqTable[c]++;
    }
    freqTable[256] = 1; // 1 occurence of PSEUDO_EOF character
    return freqTable;
}

// Accept a frequency table and use it to create a Huffman encoding tree
// based on those frequencies. We return a pointer to the node representing the root
// of the tree.
HuffmanNode* buildEncodingTree(const Map<int, int>& freqTable) {
    PriorityQueue<HuffmanNode*> pq; // store all the nodes in the priority queue.
    Vector<int> key = freqTable.keys();
    for (int i = 0; i < key.size(); i++){
        HuffmanNode *newNode = new HuffmanNode(key[i],freqTable[key[i]],nullptr,nullptr);
        pq.add(newNode,newNode->count); // lower priority numbers correspond to higher priorities.
    }
    while (pq.size() > 1){
        HuffmanNode *newNode = new HuffmanNode();
        HuffmanNode *node1 = pq.dequeue();
        HuffmanNode *node2 = pq.dequeue();
        newNode->count = node1->count + node2->count;
        newNode->zero = node1;
        newNode->one = node2;
        pq.add(newNode,newNode->count);
    }
    HuffmanNode *root = pq.dequeue();
    return root;
}

// A recursive helper function for buildEncodingMap(). This helps fill up our encodingMap.
// Note that we are actually using recursive backtracking.
void buildEncodingMapHelper(HuffmanNode *encodingTree,Map<int,string> &encodingMap,string build){
    if (encodingTree->zero == nullptr && encodingTree->one == nullptr){ // we reached a node that contains the character
        encodingMap[encodingTree->character] = build;
        return;
    }
    build += "0";
    buildEncodingMapHelper(encodingTree->zero,encodingMap,build);
    build.pop_back();
    build += "1";
    buildEncodingMapHelper(encodingTree->one,encodingMap,build);
    build.pop_back();
}


// We accept a pointer to the root node of the Huffman tree and use it to create and
// return a Huffman encoding map based on the tree's structure. The Huffman encoding map
// has character represented as int as the key and the binary representation of the encoding
// as a string data type as the value.
Map<int, string> buildEncodingMap(HuffmanNode* encodingTree) {
    Map<int, string> encodingMap;
    buildEncodingMapHelper(encodingTree,encodingMap,""); // call recursive helper function.
    return encodingMap;
}

// We read one character at a time from a given input file, and use the provided encoding map to
// encode each character to binary, then write the character's encoded binary bits to the given bit
// output stream. After writing the file's contents we write a single occurrence of the binary encoding
// for PSEUDO_EOF into the output so that we can identify the end of the data when decompressing the file later.
void encodeData(istream& input, const Map<int, string>& encodingMap, obitstream& output) {
    int c;
    while (true){
        c = input.get();
        if (c == -1){ // we reached the end of input stream.
            break;
        }
        string binaryEncoding = encodingMap[c];
        for (int i = 0; i < binaryEncoding.size(); i++){
            binaryEncoding[i] == '1' ? output.writeBit(1) : output.writeBit(0);
        }
    }
    string eofEncoding = encodingMap[256];
    for (int i = 0; i < eofEncoding.size(); i++){ // adding occurence of PSEUDO_EOF at the end of the file.
        eofEncoding[i] == '1' ? output.writeBit(1) : output.writeBit(0);
    }
}

// recursive helper function for decodeData().
int decodeDataHelper(ibitstream &input,HuffmanNode *encodingTree){
    if (encodingTree->zero == nullptr && encodingTree->one == nullptr){ // we have reached a leaf node.
        return encodingTree->character;
    }
    int val = input.readBit();
    if (val == -1){ // this means that we reached the end of the stream.
        return -1;
    }
    if (val== 0){
        return decodeDataHelper(input,encodingTree->zero); // move left
    }
    return decodeDataHelper(input,encodingTree->one); // move right
}

// We do the opposite of encodeData; we read bits from the given input file one
// at a time, and recursively walk through the specified decoding tree to write the original
// uncompressed contents of that file to the given output stream.
void decodeData(ibitstream& input, HuffmanNode* encodingTree, ostream& output) {
    while (true){
        int ch = decodeDataHelper(input,encodingTree);
        if (ch == 256 || ch == -1){ // reached EOF or reached end of input.
            break;
        }
        output << char(ch);
    }
}



// Read the input file one character at a time, building an encoding of its contents,
// and write a compressed version of that input file, including a header, to the specified
// output file. This function is built on top of the other encoding functions and
// calls them as needed.
void compress(istream& input, obitstream& output) {
    Map<int,int> freqTable = buildFrequencyTable(input); // build frequency table first.
    output << freqTable; // write the map into the bit output stream first before writing bits into the compressed file.
    HuffmanNode *temp = buildEncodingTree(freqTable); // we get a pointer to the root of the tree
    Map<int,string> encodingMap = buildEncodingMap(temp); // construct encoding map
    rewindStream(input); // we rewind the stream for encoding
    encodeData(input,encodingMap,output);
    freeTree(temp); // avoiding memory leak.
}

// We perform the opposite of compress(). We read the bits from the given input file
// one at a time, including the header packed inside the start of the file to write the original
// contents of that file to the file specified by the output parameter.
void decompress(ibitstream& input, ostream& output) {
    Map<int,int> freqTable; // we have to construct the freqTable from the first "x" bits that represent the freqTable.
    input >> freqTable; // read in the map first
    HuffmanNode *temp = buildEncodingTree(freqTable); // get encoding tree for decoding data.
    decodeData(input,temp,output);
    freeTree(temp); // avoiding memory leak.
}

// In this function we free the memory associated with the tree whose root node
// is represented by the given pointer. We free the root node and all nodes in its subtrees.
// It is called in other functions for freeing dynamically allocated memory.
// We use post order traversal to delete the nodes.
void freeTree(HuffmanNode* node) {
    if (node == nullptr){
        return;
    }
    freeTree(node->zero);
    freeTree(node->one);
    delete node;
}
