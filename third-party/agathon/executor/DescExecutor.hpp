#ifndef DESCEXECUTOR_HPP
#define DESCEXECUTOR_HPP

#include <RegularSQLExecutor.hpp>

class DescExecutor : public RegularSQLExecutor{

    private:
        DataDictionary *dic;

    private:
        DataDictionary *dictionary();

    public:
        DescExecutor(string sql, DatabaseManager *dbmg) : RegularSQLExecutor(sql, dbmg){
            dic = new DataDictionary(dbmg);
        }
        ~DescExecutor();

        vector<string> translate();
        bool isExecutable();
};

#endif // DESCEXECUTOR_HPP
