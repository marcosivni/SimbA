#ifndef PARSETREE_HPP
#define PARSETREE_HPP

#include <vector>
#include <ACTLine.hpp>
#include <ParseTreeBlock.hpp>
#include <iostream>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <cstdio>

using namespace std;

class ParseTree {

private:
    vector < ACTLine > act;
    vector < ParseTreeBlock > lines;

public:
    ParseTree();
    ~ParseTree();

    int addACTLine(ACTLine actLine);
    int addParseTreeBlock(ParseTreeBlock parseTreeBlock);
    int addParseTreeBlock(ParseTreeBlock parseTreeBlock, ACTLine actLine);

    ACTLine getACTLine(int id);
    ParseTreeBlock getParseTreeBlock(int id);
    //"Size" may be strange? (could be two block on the same level)
    // Suggestion: getNumberOfBlocks or countBlocks / countLines
    int getPTSize();
    int getACTSize();

    //New generic methods
    string toString();
    bool isEqual(ParseTree *pt);
    void clear();
    bool isEmpty();

    unsigned char *serialize();
    void unserialize(unsigned char *serialized);

    string serializeToString();
    void unserializeFromString();

    //The method below will be moved to "CatalogDictionary" after definition
    vector <string> persistParseTree();
};

#endif // PARSETREE_HPP
