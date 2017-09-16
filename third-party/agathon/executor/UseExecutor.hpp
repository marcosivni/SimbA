#ifndef USEEXECUTOR_HPP
#define USEEXECUTOR_HPP

#include <RegularSQLExecutor.hpp>

class UseExecutor : public RegularSQLExecutor{

    private:
        DataDictionary *dic;

    public:
        UseExecutor(string sql, DatabaseManager *dbmg) : RegularSQLExecutor(sql, dbmg){
            dic = new DataDictionary(dbmg);
        }

        ~UseExecutor();

        vector<string> translate();
        bool isExecutable();
};

#endif // USEEXECUTOR_HPP
