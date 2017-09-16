#ifndef SHOWEXECUTOR_HPP
#define SHOWEXECUTOR_HPP

#include <RegularSQLExecutor.hpp>

class ShowExecutor : public RegularSQLExecutor{    

    private:
        DataDictionary *dic;

    private:
        DataDictionary *dictionary();

    public:
        ShowExecutor(string sql, DatabaseManager *dbmg) : RegularSQLExecutor(sql, dbmg){
            dic = new DataDictionary(dbmg);
        }
        ~ShowExecutor();

        vector<string> translate();
        bool isExecutable();
};

#endif // SHOWEXECUTOR_HPP
