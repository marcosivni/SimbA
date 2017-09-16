#ifndef DROPMETRICEXECUTOR_HPP
#define DROPMETRICEXECUTOR_HPP

#include <RegularSQLExecutor.hpp>
#include <cstdio>

class DropMetricExecutor : public RegularSQLExecutor{

    private:
        DataDictionary *dic;

    private:
        DataDictionary *dictionary();

    public:
        DropMetricExecutor(string sql, DatabaseManager *dbmg) : RegularSQLExecutor(sql, dbmg){
            dic = new DataDictionary(dbmg);
        }
        ~DropMetricExecutor();

        vector<string> translate();
        bool isExecutable();
};



#endif // DROPMETRICEXECUTOR_HPP
