#ifndef DROPINDEXEXECUTOR_HPP
#define DROPINDEXEXECUTOR_HPP

#include <RegularSQLExecutor.hpp>
#include <fstream>
#include <cstdio>

class DropIndexExecutor : public RegularSQLExecutor{

    private:
        DataDictionary *dic;

    private:
        DataDictionary *dictionary();

    public:
        DropIndexExecutor(string sql, DatabaseManager *dbmg) : RegularSQLExecutor(sql, dbmg){
            dic = new DataDictionary(dbmg);
        }
        ~DropIndexExecutor();

        vector<string> translate();
        bool isExecutable();
};

#endif // DROPINDEXEXECUTOR_HPP
