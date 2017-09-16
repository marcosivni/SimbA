#include "CreateMetricExecutor.hpp"

CreateMetricExecutor::~CreateMetricExecutor(){

    if (dic != NULL){
        delete (dic);
    }
}

DataDictionary * CreateMetricExecutor::dictionary(){

    return dic;
}


void CreateMetricExecutor::extractor_name(){

    checkExtractors->addExtractor(currentToken().getLexem());
    getParser()->match(getParser()->getCurrentToken());
}


void CreateMetricExecutor::parameter_reference(){

    checkExtractors->addParameter(checkExtractors->getExtractor(checkExtractors->getSize()-1).first, currentToken().getLexem());
    getParser()->match(getParser()->getCurrentToken());

    if (currentToken().toUpperLexem() == "AS") {
        getParser()->match(getParser()->getCurrentToken());
        getParser()->match(getParser()->getCurrentToken());
    } else {
        if (currentToken().getTokenType() == Token::TK_IDENTIFIER) {
            checkExtractors->addExtractorAlias(currentToken().getLexem());
            getParser()->match(getParser()->getCurrentToken());
        }
    }
    if ((currentToken().getTokenType() == Token::TK_FLOATING_POINT) ||
            (currentToken().getTokenType() == Token::TK_INTEGER)) {
        checkExtractors->addWeigth(currentToken().getLexem().c_str());
        getParser()->match(getParser()->getCurrentToken());
    } else {
        checkExtractors->addWeigth("1");
    }
}

void CreateMetricExecutor::association(){

    extractor_name();
    getParser()->match("(", Token::TK_OPEN_BRACE);
    parameter_reference();
    getParser()->match(")", Token::TK_CLOSE_BRACE);
}

void CreateMetricExecutor::associations_list(){

    association();

    if (currentToken().getTokenType() == Token::TK_COMMA){
        getParser()->match(getParser()->getCurrentToken());
        associations_list();
    }
}

string CreateMetricExecutor::additionalInsertIntoMetricStruct(string metricName, string dataType, string distanceCode, string metricCode){

    string aux;

    // @loc
    if(dbmg->getSGBD() == DatabaseManager::POSTGRES)
    {
        aux =  "INSERT INTO \"CDD$MetricStruct\" VALUES ( '";
    }
    else
    {
        aux =  "INSERT INTO CDD$MetricStruct VALUES ( '";
    }

    aux += metricName + "', UPPER('"+ dataType + "'), ";
    aux += distanceCode + ", " + metricCode + " )";

    return aux;
}

vector<string> CreateMetricExecutor::additionalInsertIntoParameterStruct(string metricCode){

    string aux;
    vector<string> answer;

    for (int x = 0; x < checkExtractors->getParametersSize(); x++){
        // @loc
        if(dbmg->getSGBD() == DatabaseManager::POSTGRES)
        {
            aux = "INSERT INTO \"CDD$ParameterStruct\" VALUES ( " + metricCode + ", ";
        }
        else
        {
            aux = "INSERT INTO CDD$ParameterStruct VALUES ( " + metricCode + ", ";
        }
        aux += dictionary()->getExtractorCode(checkExtractors->getParameter(x).first) + ", '";
        aux += checkExtractors->getParameter(x).second + "', '";
        for (int y = 0; y < checkExtractors->getSize(); y++){
            if (checkExtractors->getExtractor(y).first == checkExtractors->getParameter(x).first)
                aux += checkExtractors->getExtractor(y).second + "', '";
        }
        aux += dictionary()->getParameterType(checkExtractors->getParameter(x).second, dictionary()->getExtractorCode(checkExtractors->getParameter(x).first)) + "', '0', '";
        aux += checkExtractors->getWeigth(x) + "')";
        answer.push_back(aux);
        aux.clear();
    }

    return answer;
}

vector<string> CreateMetricExecutor::translate(){

    vector<string> answer;
    string metricName, distanceFunction, dataType;
    string metricCode = dictionary()->getNewMetricId();

    try{
        getParser()->match(getParser()->getCurrentToken()); //create
        getParser()->match(getParser()->getCurrentToken()); //metric
        metricName = currentToken().getLexem();
        getParser()->match(getParser()->getCurrentToken()); //metric_name
        if (currentToken().toUpperLexem() == "USING"){
            getParser()->match(getParser()->getCurrentToken()); //using
            distanceFunction = dictionary()->getDistanceFunctionCode(currentToken().getLexem());
            getParser()->match(getParser()->getCurrentToken()); //distance function
        } else {
            distanceFunction = dictionary()->getDefaultDistanceFunctionCode();
        }
        getParser()->match(getParser()->getCurrentToken()); //for
        dataType = currentToken().getLexem();
        getParser()->match(getParser()->getCurrentToken()); //complex data type
        getParser()->match("(", Token::TK_OPEN_BRACE);
        associations_list();
        getParser()->match(")", Token::TK_CLOSE_BRACE);
    } catch (...) {
    }

    answer.push_back(additionalInsertIntoMetricStruct(metricName, dataType, distanceFunction, metricCode));

    vector<string> aux = additionalInsertIntoParameterStruct(metricCode);
    for (int x = 0; x < aux.size(); x++){
        answer.push_back(aux[x]);
        aux[x].clear();
    }
    aux.clear();

    metricName.clear();
    distanceFunction.clear();
    dataType.clear();

    delete (checkExtractors);
    checkExtractors = new CheckExtractor();

    getParser()->resetParser();

    return answer;
}

bool CreateMetricExecutor::isExecutable(){

    return (translate().size() > 0);
}
