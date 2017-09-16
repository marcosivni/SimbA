#ifndef SYNTACTICCREATEINDEXANALYZER_HPP
#define SYNTACTICCREATEINDEXANALYZER_HPP

#include <SyntacticRegularSQLAnalyzer.hpp>

class SyntacticCreateIndexAnalyzer : public SyntacticRegularSQLAnalyzer{

    private:
        DataDictionary *dic;
        string tableName;
        vector<string> columnName;
        string indexName;

    private:
        void index_name();
        void table_name();
        void column_name();
        void metric_name();
        void create_index_specification();

        string getTableName();
        string getColumnName(int pos) throw (std::exception*);
        string getIndexName();
        DataDictionary *getDictionary();

    public:
        SyntacticCreateIndexAnalyzer(string sql, DatabaseManager *dbmg) : SyntacticRegularSQLAnalyzer (sql, dbmg){
            dic = new DataDictionary(dbmg);
        }

        ~SyntacticCreateIndexAnalyzer();

        bool isValid();
};

#endif // SYNTACTICCREATEINDEXANALYZER_HPP
