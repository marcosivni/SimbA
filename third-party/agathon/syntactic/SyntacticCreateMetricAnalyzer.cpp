#include "SyntacticCreateMetricAnalyzer.hpp"

DataDictionary* SyntacticCreateMetricAnalyzer::getDictionary(){

    return dic;
}

SyntacticCreateMetricAnalyzer::~SyntacticCreateMetricAnalyzer(){

    if (dic != NULL){
        delete (dic);
    }

    if (checkExtractors != NULL){
        delete (checkExtractors);
    }
}

void SyntacticCreateMetricAnalyzer::metric_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if (getDictionary()->isMetric(currentToken().getLexem())){
            addError("Metric '" + currentToken().getLexem() + "' already exists!");
        } else {
            getParser()->match(getParser()->getCurrentToken());
        }
    } else {
        addError("Metric name expected. '" + currentToken().getLexem() + "' found instead.");
    }
}

void SyntacticCreateMetricAnalyzer::distance_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if (getDictionary()->isDistanceFunctionName(currentToken().getLexem())){
            getParser()->match(getParser()->getCurrentToken());
        } else {
            addError(currentToken().getLexem() + " does not a valid identifier for a distance funtion.");
        }
    } else {
        addError("Identifier expected '" + currentToken().getLexem() + "' found instead.");
    }
}

void SyntacticCreateMetricAnalyzer::datatype_name(){

    bool found = false;
    string types;

    vector<string> dataTypes = getDictionary()->getAllValidDataTypes();

    for (int x = 0; x < dataTypes.size(); x++){
        if (dataTypes[x] == currentToken().toUpperLexem()){
            found = true;
            checkExtractors->setDataType(currentToken().toUpperLexem());
            break;
        }
    }

    for (int x = 0; x < dataTypes.size(); x++){
        if (x > 0){
            types += " | ";
        }
        types += dataTypes[x];
        dataTypes[x].clear();
    }
    dataTypes.clear();

    if (found) {
        getParser()->match(getParser()->getCurrentToken());
    } else {
        addError("'" + currentToken().getLexem() + "' is a invalid data type.");
        addError("Valid types are ( " + types + " )");
        types.clear();
    }
}

void SyntacticCreateMetricAnalyzer::extractor_name(){

    if (getDictionary()->isValidExtractorForThisType(checkExtractors->getDataType(), currentToken().toUpperLexem())){
        checkExtractors->addExtractor(currentToken().getLexem());
        getParser()->match(getParser()->getCurrentToken());
    } else {
        addError("'" + currentToken().getLexem() + "' is not a valid extractor for " + checkExtractors->getDataType() + ".");
    }
}

void SyntacticCreateMetricAnalyzer::parameter_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER) {
        // Verifies if the parameter exists
        if (!getDictionary()->isExtractorParameter(atoi(getDictionary()->getExtractorCode(checkExtractors->getExtractor(checkExtractors->getSize()-1).first).c_str()), currentToken().getLexem())) {
            addError("Invalid parameter name: " + currentToken().getLexem());
        } else {
            getParser()->match(getParser()->getCurrentToken());
        }
    } else {
        addError("Missing or invalid extractor - parameter name after '('.");
    }
}

void SyntacticCreateMetricAnalyzer::parameter_reference(){

    parameter_name();

    if (currentToken().toUpperLexem() == "AS") {
        getParser()->match(getParser()->getCurrentToken());
        if (currentToken().getTokenType() == Token::TK_IDENTIFIER) {
            // @loc - added next line, similar to else condition...
            checkExtractors->addExtractorAlias(currentToken().getLexem());
            getParser()->match(getParser()->getCurrentToken());
        } else {
            addError("Identifier expected after AS clause.");
        }
    } else {
        if (currentToken().getTokenType() == Token::TK_IDENTIFIER) {
            checkExtractors->addExtractorAlias(currentToken().getLexem());
            getParser()->match(getParser()->getCurrentToken());
        }
    }
    if ((currentToken().getTokenType() == Token::TK_FLOATING_POINT) ||
        (currentToken().getTokenType() == Token::TK_INTEGER)) {
        getParser()->match(getParser()->getCurrentToken());
    }
}

void SyntacticCreateMetricAnalyzer::association(){

    extractor_name();
    getParser()->match("(", Token::TK_OPEN_BRACE);
    parameter_reference();
    getParser()->match(")", Token::TK_CLOSE_BRACE);
}

void SyntacticCreateMetricAnalyzer::associations_list(){

    association();

    if (currentToken().getTokenType() == Token::TK_COMMA){
        getParser()->match(getParser()->getCurrentToken());
        associations_list();
    }
}

void SyntacticCreateMetricAnalyzer::create_metric_specification(){

    try{
        if (currentToken().toUpperLexem() == "CREATE"){
            getParser()->match(getParser()->getCurrentToken());
            if (currentToken().toUpperLexem() == "METRIC"){
                getParser()->match(getParser()->getCurrentToken());
                metric_name();
                if (currentToken().toUpperLexem() == "USING"){
                    getParser()->match(getParser()->getCurrentToken());
                    distance_name();
                }
                if (currentToken().toUpperLexem() == "FOR"){
                    getParser()->match(getParser()->getCurrentToken());
                    datatype_name();
                    getParser()->match("(", Token::TK_OPEN_BRACE);
                    associations_list();
                    getParser()->match(")", Token::TK_CLOSE_BRACE);
                } else {
                    addError("Missing FOR clause for CREATE METRIC statement.");
                }
                if (!endOfStatement()){
                    addError("Extra tokens after the end of statement: '" + currentToken().getLexem() + "'.");
                }
            } else {
                addError("Missing METRIC after CREATE clause.");
            }
        } else {
            addError("Missing CREATE clause.");
        }
    } catch (...) {
        addError("Syntax Error on CREATE METRIC statement near '"+ getParser()->getToken(getParser()->countTokens()-1)->getLexem() +"'.");
    }
}

bool SyntacticCreateMetricAnalyzer::isValid(){

    clearErrors();

    create_metric_specification();

    if (!checkExtractors->isValid()){
        addError(checkExtractors->getError());
    }

    delete (checkExtractors);
    checkExtractors = new CheckExtractor();

    getParser()->resetParser();

    return (!hasErrors());
}
