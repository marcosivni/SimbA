#ifndef SYNTACTICSELECTANALYZER_HPP
#define SYNTACTICSELECTANALYZER_HPP

#include <fstream>
#include <iostream>
#include <SyntacticRegularSQLAnalyzer.hpp>
#include <SelectSemanticStructs.hpp>
#include <ComplexObject.hpp>

using namespace std;

class SyntacticSelectAnalyzer : public SyntacticRegularSQLAnalyzer{

    private:
        DataDictionary *dic;
        CheckValidTables *listTables;
        CheckValidColumns *listColumns;
        bool is, iext;

    public:
        static const bool FLAG_IGNORE_SEMANTICS = true;
        static const bool FLAG_IGNORE_EXTRA_TOKENS = true;

    private:
        DataDictionary *getDictionary();
        vector<string> getTableNameFromTokenList(vector<Token*> tokenList, string attribName);

        //Methods for Syntactic
        void select_list();
        void column_reference_list();
        void column_reference(int op = 1);
        void asterisk_name();
        void as_clause(int op = 0);
        void table_expression();
        void from_clause();
        void table_reference_list();
        void where_clause();
        void groupby_clause();
        void having_clause();
        void search_condition(int op = 0);
        void predicate(int op = 0);
        void value_expression();
        void near_or_far_predicate();
        void query_obj_definition();
        void metric_name();
        void val_range();
        void k_nearest();
        void tie_list();
        void query_specification();
        string par_processing();
        void process_join();
        void read_table();
        void process_similarity_join();
        void join_reference_list();
        void join_reference();
        void complex_predicate();
        void regular_predicate();
        void in_clause();
        void between_clause();
        void value_check();
        void order_by_clause();

        //Methods for Semantic
        void setFlagIgnoreSemantics(bool ignoreSemantics);
        void setFlagIgnoreExtraTokens(bool ignoreExtraTokens);
        void addTable(string tableName, string alias = "");
        void addTableAlias(string alias);
        void addColumn(string column, string tableName = "");
        bool flagIgnoreSemantics();
        bool flagIgnoreExtraTokens();
        int countNumberOfProjections(string subselect);

    public:
        SyntacticSelectAnalyzer(string sql, DatabaseManager *dbmg, bool ignoreSemantics = false, bool ignoreExtraTokens = false) : SyntacticRegularSQLAnalyzer (sql, dbmg){

            setFlagIgnoreSemantics(ignoreSemantics);
            setFlagIgnoreExtraTokens(ignoreExtraTokens);
            dic = new DataDictionary(dbmg);
            listTables = new CheckValidTables(dic);
            listColumns = new CheckValidColumns(dic);
        }

        ~SyntacticSelectAnalyzer();

        bool isValid();
};

#endif // SYNTACTICSELECTANALYZER_HPP
