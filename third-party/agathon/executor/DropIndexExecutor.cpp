#include "DropIndexExecutor.hpp"

DropIndexExecutor::~DropIndexExecutor(){

    if (dic != NULL){
        delete (dic);
    }
}

DataDictionary* DropIndexExecutor::dictionary(){

    return dic;
}

vector<string> DropIndexExecutor::translate(){

    vector<string> answer;

    try{
        bool complex = false;
        string tableName = getParser()->getToken(2)->getLexem();
        string idxName = getParser()->getToken(4)->getLexem();

        if (dictionary()->getIndexFile(tableName, idxName) == "NULL"){
            answer.push_back(dictionary()->dropIndexStatement(tableName, idxName));
        } else {
            answer.push_back("DELETE FROM CDD$ComplexAttribMetric WHERE IndexName='" + idxName + "'");

            string indexFile = dictionary()->getIndexFile(tableName, idxName);
            if (!(indexFile == "NULL")){
                remove(("siren_slim_" + indexFile).c_str());
                remove(("siren_dummy_" + indexFile).c_str());
            }
        }
    } catch (...){
    }

    getParser()->resetParser();

    return answer;
}

bool DropIndexExecutor::isExecutable(){

    return (translate().size() > 0);
}
