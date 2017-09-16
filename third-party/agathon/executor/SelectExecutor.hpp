#ifndef SELECTEXECUTOR_HPP
#define SELECTEXECUTOR_HPP

#include <RegularSQLExecutor.hpp>
#include <ComplexObject.hpp>
#include <IndexManager.hpp>
#include <SirenExtractors.hpp>
#include <Evaluator.hpp>
#include <SelectSemanticStructs.hpp>

class SelectExecutor : public RegularSQLExecutor{

    private:
        DataDictionary *dic;
        CheckValidTables *listTables;
        CheckValidColumns *listColumns;

    private:
        DataDictionary *dictionary();

        //Methods for Syntactic
        void select_list();
        void column_reference_list();
        void column_reference();
        void asterisk_name();
        void as_clause(int op = 0);
        void table_expression();
        void from_clause();
        void table_reference_list();
        void where_clause();
        void groupby_clause();
        void having_clause();
        void search_condition();
        void predicate();
        void value_expression();
        void near_or_far_predicate();
        void query_obj_definition();
        void metric_name();
        void val_range();
        void k_nearest();
        void tie_list();
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
        int countNumberOfProjections(string subselect);

        //Translated Methods
        void morph_complex_to_regular_selection();
        void morph_asterisk_to_regular_columns();
        void morph_complex_to_regular_columns();
        void morph_complex_to_regular_tables(string tab, string alias, string col);

        string discoverTableName(string columnName);
        Signature *getCenterSignature(vector<string> centerNames, string metricName, string dataType);
        Signature **getCentersSignatures(vector<string> centerNames, string metricName, string dataType);

    public:
        SelectExecutor(string sql, DatabaseManager *dbmg) : RegularSQLExecutor(sql, dbmg){
            dic = new DataDictionary(dbmg);
            listTables = new CheckValidTables(dic);
            listColumns = new CheckValidColumns(dic);
        }

        ~SelectExecutor();

        vector<string> translate();
        bool isExecutable();
};

#endif // SELECTEXECUTOR_HPP
