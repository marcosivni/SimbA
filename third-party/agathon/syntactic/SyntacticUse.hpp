#ifndef SYNTACTICUSE_HPP
#define SYNTACTICUSE_HPP

#include <SyntacticRegularSQLAnalyzer.hpp>

class SyntacticUse : public SyntacticRegularSQLAnalyzer{

    private:
        DataDictionary *dic;

    private:
        DataDictionary *dictionary();

    public:
        SyntacticUse (string sql, DatabaseManager *dbmg):SyntacticRegularSQLAnalyzer (sql, dbmg){
            dic = new DataDictionary(dbmg);
        }
        ~SyntacticUse ();

        bool isValid();
};

#endif // SYNTACTICUSE_HPP
