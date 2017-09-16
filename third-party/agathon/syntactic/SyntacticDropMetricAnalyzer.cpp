#include "SyntacticDropMetricAnalyzer.hpp"

SyntacticDropMetricAnalyzer::~SyntacticDropMetricAnalyzer(){

    if (dic != NULL){
        delete (dic);
    }
}

void SyntacticDropMetricAnalyzer::metric_name(){

    if (currentToken().getTokenType() == Token::TK_IDENTIFIER){
        if (dic->isMetric(currentToken().getLexem())){
            getParser()->match(getParser()->getCurrentToken());
        } else {
            addError("Metric '" + currentToken().getLexem() + "' does not exists.");
        }
    } else {
        addError("Identifier expected '" + currentToken().getLexem() + "' found instead.");
    }
}


void SyntacticDropMetricAnalyzer::drop_metric_specification(){

    try{
        if (currentToken().toUpperLexem() == "DROP"){
            getParser()->match(getParser()->getCurrentToken());
            if (currentToken().toUpperLexem() == "METRIC"){
                getParser()->match(getParser()->getCurrentToken());
                metric_name();
                if (!endOfStatement()){
                    addError("Extra tokens at the end of statement: " + currentToken().getLexem() + "'.");
                }
            } else {
                addError("METRIC is expected. '" + currentToken().getLexem() + "' found instead.");
            }
        } else {
            addError("DROP METRIC statement is expected. '" + currentToken().getLexem() + "' found instead.");
        }
    } catch (...) {
        addError("Syntax error when processing DROP METRIC near '"+ getParser()->getToken(getParser()->countTokens()-1)->getLexem() +"'.");
    }
}

DataDictionary* SyntacticDropMetricAnalyzer::dictionary(){

    return dic;
}

bool SyntacticDropMetricAnalyzer::isValid(){

    clearErrors();

    drop_metric_specification();
    getParser()->resetParser();

    return (!hasErrors());
}
