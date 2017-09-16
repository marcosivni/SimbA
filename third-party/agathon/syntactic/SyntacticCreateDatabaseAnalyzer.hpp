#ifndef SYNTACTICCREATEDATABASEANALYZER_HPP
#define SYNTACTICCREATEDATABASEANALYZER_HPP

#include <SyntacticRegularSQLAnalyzer.hpp>

class SyntacticCreateDatabaseAnalyzer : public SyntacticRegularSQLAnalyzer{

    private:
        DataDictionary *dic;

    private:
        DataDictionary *dictionary();

    public:
        SyntacticCreateDatabaseAnalyzer(string sql, DatabaseManager *dbmg):SyntacticRegularSQLAnalyzer (sql, dbmg){
            dic = new DataDictionary(dbmg);
        }
        ~SyntacticCreateDatabaseAnalyzer();

        bool isValid();
};



#endif // SYNTACTICCREATEDATABASEANALYZER_HPP
