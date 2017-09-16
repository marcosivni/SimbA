#ifndef REGULARSQLEXECUTOR_HPP
#define REGULARSQLEXECUTOR_HPP

#include <cstring>
#include <iostream>
#include <cstdio>
#include <vector>
#include <DataDictionary.hpp>
#include <Parser.hpp>

using namespace std;

class RegularSQLExecutor
{
    // @loc
    protected:
        DatabaseManager *dbmg;

    private:
        string exp;
        Parser *p;

    public:
        RegularSQLExecutor(DatabaseManager *dbmg);
        RegularSQLExecutor(string expression, DatabaseManager *dbmg);
        ~RegularSQLExecutor();

        void setSQLExpression(string sqlExpression);
        string getSQLExpression();
        Parser *getParser();
        Token currentToken();

        bool isExecutable();
        bool endOfStatement();
};

#endif // REGULARSQLEXECUTOR_HPP
