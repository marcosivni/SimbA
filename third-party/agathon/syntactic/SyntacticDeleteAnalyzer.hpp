#ifndef SYNTACTICDELETEANALYZER_HPP
#define SYNTACTICDELETEANALYZER_HPP

#include <SyntacticRegularSQLAnalyzer.hpp>

class SyntacticDeleteAnalyzer : public SyntacticRegularSQLAnalyzer{

    private:
        DataDictionary *dic;
        string tableName;

    private:
        void table_name();
        void where_clause();
        void column_name();
        void value_name();
        void column_reference();
        void column_reference_list();
        void delete_specification();

        DataDictionary *dictionary();
        string getTableName();

    public:
        SyntacticDeleteAnalyzer(string sql, DatabaseManager *dbmg):SyntacticRegularSQLAnalyzer (sql, dbmg){

            dic = new DataDictionary(dbmg);
        }

        ~SyntacticDeleteAnalyzer();

        bool isValid();
};

#endif // SYNTACTICDELETEANALYZER_HPP
