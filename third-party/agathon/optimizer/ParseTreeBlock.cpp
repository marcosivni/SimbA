#include "ParseTreeBlock.hpp"

ParseTreeBlock::ParseTreeBlock(){

    setOperation(-1);
    setLeftEdge(-1);
    setRightEdge(-1);
    setCost(-1);
    setACTReference(-1);
}

ParseTreeBlock::~ParseTreeBlock(){

}

void ParseTreeBlock::setOperation(int operation){

    this->operation = operation;
}

void ParseTreeBlock::setTable(string table){

    this->table = table;
}

void ParseTreeBlock::setLeftEdge(int leftEdge){

    this->leftEdge = leftEdge;
}


void ParseTreeBlock::setRightEdge(int rightEdge){

    this->rightEdge = rightEdge;
}

void ParseTreeBlock::setCost(double cost){

    this->cost = cost;
}

void ParseTreeBlock::setACTReference(int actReference){

    this->actReference = actReference;
}

int ParseTreeBlock::getOperation(){

    return operation;
}

string ParseTreeBlock::getTable(){

    return table;
}

int ParseTreeBlock::getLeftEdge(){

    return leftEdge;
}

int ParseTreeBlock::getRightEdge(){

    return rightEdge;
}

double ParseTreeBlock::getCost(){

    return cost;
}

int ParseTreeBlock::getACTReference(){

    return actReference;
}

bool ParseTreeBlock::isEqual(ParseTreeBlock *pt){

    return( (getOperation() == pt->getOperation()) &&
            (getTable() == pt->getTable()) &&
            (getLeftEdge() == pt->getLeftEdge()) &&
            (getRightEdge() == pt->getRightEdge()) &&
            (getCost() == pt->getCost()) &&
            (getACTReference() == pt->getACTReference()) );
}
