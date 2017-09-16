#ifndef SYNTACTICDROPINDEXANALYZER_HPP
#define SYNTACTICDROPINDEXANALYZER_HPP

#include <SyntacticRegularSQLAnalyzer.hpp>

class SyntacticDropIndexAnalyzer : public SyntacticRegularSQLAnalyzer{

    private:
        DataDictionary *dic;
        string tableName;

    private:
        void index_name();
        void table_name();
        void drop_index_specification();

        DataDictionary *getDictionary();
        string getTableName();

    public:
        SyntacticDropIndexAnalyzer(string sql, DatabaseManager *dbmg)  : SyntacticRegularSQLAnalyzer (sql, dbmg){
            dic = new DataDictionary(dbmg);
        }

        bool isValid();
};

#endif // SYNTACTICDROPINDEXANALYZER_HPP
