#ifndef SELECTSEMANTICSTRUCTS_HPP
#define SELECTSEMANTICSTRUCTS_HPP

#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <DataDictionary.hpp>

using namespace std;

class CheckValidTables{

    private:
        vector< pair<string, string> > listTables;
        DataDictionary *dic;
        string error;

    public:
        CheckValidTables(DataDictionary *dic);
        ~CheckValidTables();

        void addNewTable(string tableName, string aliasName);
        void addNewTable(string tableName);
        void addNewAlias(string alias);
        void addError(string error);

        int countTableAliases(string tableName);
        int countTable(string alias);

        bool isValid();
        bool isUniqueAliases();

        string tableAlias(string tableName);
        string tableName(string alias);
        DataDictionary *dictionary();

        int getSize();
        pair<string, string> getTableNameAndAlias(int pos) throw (std::exception*);
        string getError();
        string toString();
};

class CheckValidColumns{

    private:
        vector< pair<string, string> > listColumns;
        DataDictionary *dic;
        string error;

    public:
        CheckValidColumns(DataDictionary *dic);
        ~CheckValidColumns();

        void addNewColumn(string tableName, string columnName);
        void addNewTable(string tableName);
        void addNewColumn(string column);
        void addError(string error);

        bool isValid(CheckValidTables *validTables);
        int countAppearences(string tableAlias, string column, CheckValidTables *validTables);

        DataDictionary *dictionary();

        int getSize();
        pair<string, string> getTableNameAndColumnName(int pos) throw (std::exception*);
        string getError();
        string toString();
};


#endif // SELECTSEMANTICSTRUCTS_HPP
