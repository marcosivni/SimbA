#ifndef SYNTACTICDROPDATABASEANALYZER_HPP
#define SYNTACTICDROPDATABASEANALYZER_HPP

#include <SyntacticRegularSQLAnalyzer.hpp>

class SyntacticDropDatabaseAnalyzer : public SyntacticRegularSQLAnalyzer{

    private:
        DataDictionary *dic;

    private:
        DataDictionary *dictionary();

    public:
        SyntacticDropDatabaseAnalyzer(string sql, DatabaseManager *dbmg):SyntacticRegularSQLAnalyzer (sql, dbmg){
            dic = new DataDictionary(dbmg);
        }
        ~SyntacticDropDatabaseAnalyzer();

        bool isValid();
};

#endif // SYNTACTICDROPDATABASEANALYZER_HPP
