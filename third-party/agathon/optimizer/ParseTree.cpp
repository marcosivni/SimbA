#include "ParseTree.hpp"

ParseTree::ParseTree(){

    act.clear();
    lines.clear();
}

ParseTree::~ParseTree(){

    act.clear();
    lines.clear();
}

int ParseTree::addACTLine(ACTLine actLine){

    act.push_back(actLine);
    return act.size() - 1;
}

int ParseTree::addParseTreeBlock(ParseTreeBlock parseTreeBlock){

    lines.push_back(parseTreeBlock);
    return lines.size() - 1;
}

int ParseTree::addParseTreeBlock(ParseTreeBlock parseTreeBlock, ACTLine actLine){

    int id = addACTLine(actLine);
    parseTreeBlock.setACTReference(id);
    return addParseTreeBlock(parseTreeBlock);
}

ACTLine ParseTree::getACTLine(int id){

    if (id < act.size())
	return act.at(id);
    else
        throw (new std::exception());
}

ParseTreeBlock ParseTree::getParseTreeBlock(int id){

    if (id < lines.size())
	return lines.at(id);
    else
        throw (new std::exception());
}

int ParseTree::getPTSize(){

    return lines.size();
}

int ParseTree::getACTSize(){

    return act.size();
}

string ParseTree::toString(){

    stringstream buffer;
    unsigned int sid, satt, sat, sop, sva, svt, spa;
    char tmp[120];

    string answer;

    sid = 0;
    satt = 0;
    sat = 0;
    sop = 0;
    sva = 0;
    svt = 0;
    spa = 0;

    for(int i = 0; i < getACTSize(); i++){
        sprintf(tmp, "%d", i);
        answer = tmp;
        if (answer.size() > sid)
            sid = answer.size()+2;
        answer.clear();

        if (getACTLine(i).getAttribute().size() > satt)
            satt = getACTLine(i).getAttribute().size()+2;

        sprintf(tmp, "%d", getACTLine(i).getAttributeType());
        answer = tmp;
        if (answer.size() > sat)
            sat = answer.size()+2;
        answer.clear();

        if (getACTLine(i).getOperator().size() > sop)
            sop = getACTLine(i).getOperator().size()+2;

        if (getACTLine(i).getValue().size() > sva)
            sva = getACTLine(i).getValue().size()+2;

        sprintf(tmp, "%d", getACTLine(i).getValueType());
        answer = tmp;
        if (answer.size() > svt)
            svt = answer.size()+2;
        answer.clear();

        if (getACTLine(i).parameterToString().size() > spa)
            spa = getACTLine(i).parameterToString().size()+2;
    }

    string line;

    if (sid <= 4)
        sid = 5;

    line += "+";
    for (int x = 0; x < sid; x++){
        line += "-";
    }
    line += "+";

    if (satt <= 10){
        satt = 11;
    }

    for (int x = 0; x < satt; x++){
        line += "-";
    }
    line += "+";

    if (sat <= 8)
        sat = 9;

    for (int x = 0; x < sat; x++){
        line += "-";
    }
    line += "+";

    if (sop <= 9)
        sop = 10;

    for (int x = 0; x < sop; x++){
        line += "-";
    }
    line += "+";

    if (sva <= 9)
        sva = 10;

    for (int x = 0; x < sva; x++){
        line += "-";
    }
    line += "+";

    if (svt <= 8)
        svt = 9;

    for (int x = 0; x < svt; x++){
        line += "-";
    }
    line += "+";

    if (spa <= 11)
        spa = 12;

    for (int x = 0; x < spa; x++){
        line += "-";
    }
    line += "+";

    answer += line;
    answer += "\n";

    answer += "|";


    if ((sid+satt+sat+sop+sva+svt+spa-33)%2 == 1){
        answer += " ";
    }
    for (int x = 0; x < (sid+satt+sat+sop+sva+svt+spa-33)/2;x++){
        answer += " ";
    }
    answer += "(ACT) - Attributes and Conditions Table";
    for (int x = 0; x < (sid+satt+sat+sop+sva+svt+spa-33)/2;x++){
        answer += " ";
    }

    answer += "|\n";

    answer += line;
    answer += "\n";

    buffer << "|" << setw(sid-1) << "id" << " |" << setw(satt-1) << "Attribute" << " |" << setw(sat-1) << "A. Type" << " |" << setw(sop-1) << "Operator" << " |" << setw(sva-1) << "Value" << " |" << setw(svt-1) << "V. Type" << " |" << setw(spa-1) << "Parameters" << " |" << endl;
    buffer << line << endl;

    for(int i = 0; i < getACTSize(); i++){
        buffer << "|" << setw(sid-1) << i << " |"  << setw(satt-1) << getACTLine(i).getAttribute() << " |" << setw(sat-1) << getACTLine(i).getAttributeType() << " |"  << setw(sop-1) << getACTLine(i).getOperator() << " |"  << setw(sva-1) << getACTLine(i).getValue() << " |" << setw(svt-1) << getACTLine(i).getValueType() << " |"  << setw(spa-1) << getACTLine(i).parameterToString();
        buffer << " |" << endl;
        buffer << line << endl;
    }
    buffer << endl;

    // Parse Tree Table
    line.clear();

    sid = 0;
    sop = 0;
    sva = 0;
    satt = 0;
    sat = 0;
    svt = 0;

    for(int i = 0; i < getPTSize(); i++){
        sprintf(tmp, "%d", i);
        line = tmp;
        if (line.size() > sid)
            sid = line.size()+2;
        line.clear();

        sprintf(tmp, "%d", getParseTreeBlock(i).getOperation());
        line = tmp;
        if (line.size() > sop)
            sop = line.size()+2;
        line.clear();

        if (getParseTreeBlock(i).getTable().size() > sva)
            sva = getParseTreeBlock(i).getTable().size()+2;

        sprintf(tmp, "%d", getParseTreeBlock(i).getLeftEdge());
        line = tmp;
        if (line.size() > satt)
            satt = line.size()+2;
        line.clear();

        sprintf(tmp, "%d", getParseTreeBlock(i).getRightEdge());
        line = tmp;
        if (line.size() > sat)
            sat = line.size()+2;
        line.clear();

        sprintf(tmp, "%d", getParseTreeBlock(i).getACTReference());
        line = tmp;
        if (line.size() > svt)
            svt = line.size()+2;
        line.clear();

    }


    if (sid <= 4)
        sid = 5;

    line += "+";
    for (int x = 0; x < sid; x++){
        line += "-";
    }
    line += "+";

    if (sop <= 10)
        sop = 11;

    for (int x = 0; x < sop; x++){
        line += "-";
    }
    line += "+";

    if (sva <= 13)
        sva = 14;

    for (int x = 0; x < sva; x++){
        line += "-";
    }
    line += "+";

    if (satt <= 10)
        satt = 11;

    for (int x = 0; x < satt; x++){
        line += "-";
    }
    line += "+";

    if (sat <= 11)
        sat = 12;

    for (int x = 0; x < sat; x++){
        line += "-";
    }
    line += "+";

    if (svt <= 14)
        svt = 15;

    for (int x = 0; x < svt; x++){
        line += "-";
    }
    line += "+";

    buffer << line << endl;

    buffer << "|";
    if ((sid+sop+sva+satt+sat+svt-11)%2 == 1){
        buffer << " ";
    }
    for (int x = 0; x < (sid+sop+sva+satt+sat+svt-11)/2; x++){
        buffer << " ";
    }
    buffer << "Parse Tree Table";
    for (int x = 0; x < (sid+sop+sva+satt+sat+svt-11)/2; x++){
        buffer << " ";
    }
    buffer << "|" << endl;
    buffer << line << endl;
    buffer << "|" <<  setw(sid-1) << "id"  << " |"  << setw(sop-1) << "Operation" << " |" << setw(sva-1) << "Table(READ)" << " |" << setw(satt-1) << "Left Edge" << " |" << setw(sat-1) << "Right Edge" << " |" << setw(svt-1) << "ACT Reference" << " |" << endl;
    buffer << line << endl;

    for (int i = 0; i < getPTSize(); i++){
        buffer << "|" << setw(sid-1) << i << " |" << setw(sop-1) << getParseTreeBlock(i).getOperation() << " |" << setw(sva-1) << getParseTreeBlock(i).getTable() << " |" << setw(satt-1) << getParseTreeBlock(i).getLeftEdge() << " |" << setw(sat-1) << getParseTreeBlock(i).getRightEdge() << " |" << setw(svt-1) << getParseTreeBlock(i).getACTReference() << " |" << endl;
        buffer << line << endl;
    }
    answer += buffer.str();
    line.clear();

    return answer;
}

bool ParseTree::isEqual(ParseTree *pt){

    if (lines.size() != pt->getPTSize())
        return false;

    for (int x = 0; x < pt->getPTSize(); x++){
        ParseTreeBlock tmp = pt->getParseTreeBlock(x);
        if (!getParseTreeBlock(x).isEqual(&tmp))
            return false;
    }

    return true;
}

void ParseTree::clear(){

    act.clear();
    lines.clear();
}

bool ParseTree::isEmpty(){

    return (lines.size() > 0);
}

unsigned char * ParseTree::serialize(){

    //Well... implement later
    //Maybe this method is not necessary if we want to persist
    //the tree INSIDE the DBMS not outside...
}

void ParseTree::unserialize(unsigned char *serialized){

    //@todo
}

string ParseTree::serializeToString(){

    //@todo
}

void ParseTree::unserializeFromString(){

    //@todo
}

//This method does not belongs to this class!!!
vector<string> ParseTree::persistParseTree(){

    //Needs catalog information definition / implementation
}
