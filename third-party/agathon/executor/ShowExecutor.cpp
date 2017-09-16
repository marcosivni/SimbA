#include "ShowExecutor.hpp"

ShowExecutor::~ShowExecutor(){

    if (dic != NULL){
        delete (dic);
    }
}

DataDictionary * ShowExecutor::dictionary(){

    return dic;
}

vector<string> ShowExecutor::translate(){

    vector<string> answer;

    try{
        getParser()->match(getParser()->getCurrentToken());

        if (currentToken().toUpperLexem() == "TABLES"){
            answer.push_back(dictionary()->showTables());
        } else {
            if (currentToken().toUpperLexem() == "EXTRACTORS"){
                answer.push_back("SELECT ExtractorName AS Extractor FROM EPD$Extractors");
            } else {
                if (currentToken().toUpperLexem() == "METRICS"){
                    answer.push_back("SELECT MetricName AS Metric FROM CDD$MetricStruct");
                } else {
                    if (currentToken().toUpperLexem() == "DISTANCE_FUNCTIONS"){
                        answer.push_back("SELECT DistanceName DistanceFunction FROM EPD$DistanceFunctions");
                    } else {
                        //Show All indexes
                        if (currentToken().toUpperLexem() == "INDEXES"){
                            answer.push_back(dictionary()->getAllTraditionalIndexesStatement());
                        } else {
                            if (currentToken().toUpperLexem() == "DATABASES"){
                                answer.push_back(dictionary()->showDatabases());
                            }
                        }
                    }
                }
            }
        }
    } catch (...){
    }

    getParser()->resetParser();

    return answer;
}

bool ShowExecutor::isExecutable(){

    return (translate().size() > 0);
}
