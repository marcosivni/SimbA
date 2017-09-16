#ifndef PARSETREEBLOCK_HPP
#define PARSETREEBLOCK_HPP

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class ParseTreeBlock{

    private:
        int operation;
        string table;
        int leftEdge;
        int rightEdge;
        int actReference;
        double cost;

    public:
        // Operation
        static const int SELECT = 0;
        static const int SIMILARITY_SELECT = 1;
        static const int JOIN = 2;
        static const int CARTESIAN_PRODUCT = 4;
        static const int PROJECTION = 5;
        static const int READ = 6;
        static const int RENAME = 7;
        static const int AGGREGATION = 8;
        static const int CLOSEST_JOIN = 9;
        static const int NEAREST_JOIN = 10;
        static const int RANGE_JOIN = 11;


    public:
        ParseTreeBlock();
        ~ParseTreeBlock();

        void setOperation(int operation);
        void setTable(string table);
        void setLeftEdge(int leftEdge);
        void setRightEdge(int rightEdge);
        void setCost(double cost);
        void setACTReference(int actReference);

        int getOperation();
        string getTable();
        int getLeftEdge();
        int getRightEdge();
        double getCost();
        int getACTReference();

        //Generic Methods
        bool isEqual(ParseTreeBlock *pt);
};

#endif // PARSETREEBLOCK_HPP
