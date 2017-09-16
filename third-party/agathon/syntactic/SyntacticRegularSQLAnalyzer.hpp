#ifndef SYNTACTICREGULARSQLANALYZER_HPP
#define SYNTACTICREGULARSQLANALYZER_HPP

#include <cstring>
#include <iostream>
#include <cstdio>
#include <vector>
#include <DatabaseManager.hpp>
#include <Parser.hpp>
#include <DataDictionary.hpp>

using namespace std;

class SyntacticRegularSQLAnalyzer{

    private:
        DatabaseManager *dbmg;
        string expression;
        vector<string> errors;
        Parser *p;

    public:
        SyntacticRegularSQLAnalyzer(DatabaseManager *dbmg);
        SyntacticRegularSQLAnalyzer(string expression, DatabaseManager *dbmg);
        ~SyntacticRegularSQLAnalyzer();

        void clearErrors();
        void setSQLExpression(string sqlExpression);
        void addError(string error);
        string getSQLExpression();
        Parser *getParser();
        Token currentToken();
        bool endOfStatement();

        bool hasErrors();
        bool isValid();
        vector<string> getErrors();
        string getError(int pos);
        int countErrors();
};

#endif // SYNTACTICREGULARSQLANALYZER_HPP
