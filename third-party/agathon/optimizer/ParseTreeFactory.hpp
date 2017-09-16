#ifndef PARSETREEFACTORY_HPP
#define PARSETREEFACTORY_HPP

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <Parser.hpp>
#include <ParseTree.hpp>

#include <QDebug>

using namespace std;

//The goal of factory is that we can create
//distinct types of Parse-Tree
class ParseTreeFactory{

    private:
        string lastSQLStatement;
        Parser *p;
        ACTLine actLine;
        ParseTreeBlock ptBlock;
        int mergeCounter;
        map<string, bool> joinedTables;
        map<string, string> rename;
        vector< pair< ParseTreeBlock, ACTLine > > joinMap;
        vector<string> tables;        
        map<string, int> bins;
        bool in_merge( string table, vector< string > mv );
        int get_ref( string table );        
        string to_lower(string str);
        ParseTree pt;

    private:
        //Follow parser direcctions
        Parser *getParser();
        Token currentToken();
        bool endOfStatement();
        string add_column(string param);
        void read_table();
        void agg_functions();
        string par_processing();
        string get_table(string table);
        void cartesian_product();
        void merge_pt(ParseTree subPT);
        void flushACTLine();
        void flushPTBlock();
        void flushParseTree();
        void merge_pt(ParseTree* subPT);

        //Follow the statement
        void select_list();
        void column_reference_list();
        void column_reference();
        void as_clause();
        void table_expression();
        void from_clause();
        void table_reference_list();
        void where_clause();
        void groupby_clause();
        void orderby_clause();
        void having_clause();
        void search_condition();
        void predicate();
        void value_expression();
        void near_predicate();
        void query_obj_definition();
        void metric_name();
        void val_range();
        void k_nearest();
        void tie_list();
        void far_predicate();
        void select_specification();
        void process_join();
        void process_similarity_join();
        void join_reference_list();
        void set_join();        
        void build();

    public:
        ParseTreeFactory();
        ~ParseTreeFactory();

        void setLastSQLStatement(string lastSQLStatement);
        void setEqualizer(int mergeCount);

        ParseTree *lastParseTree();
        ParseTree *canonicalParseTree(string sqlExpression = "");

        string getLastSQLStatement();
        int getEqualizer();
};

#endif // PARSETREEFACTORY_HPP
