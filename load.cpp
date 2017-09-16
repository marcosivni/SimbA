#include <DatabaseManager.hpp>
#include <ComplexSQLExecutor.hpp>
#include <InsertIntoExecutor.hpp>
#include <SyntacticComplexSQLAnalyzer.hpp>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

int main(int argc, char ** argv)
{
    DatabaseManager *db = new DatabaseManager(
        "postgres",
        "olmes",
        "localhost",
        DatabaseManager::POSTGRES,
        "34567",
        "SIREN" );

    ifstream f("/home/olmes/load2.txt");

    char buff[400];
    int k = 381;

    while(f)
    {
        f.getline(buff, 380);
        string input(buff);
        input = input.substr(0, input.find(';'));

        //ComplexSQLExecutor *exec = new ComplexSQLExecutor(input, db);
        InsertIntoExecutor *exec = new InsertIntoExecutor(input, db);
        vector<string> sql = exec->translate();

        for(int i = 0; i < sql.size(); i++)
        {
            //cout << sql[i] << endl;
            db->runInsertStatement(sql[i]);
        }

        cout << k << endl;
        k++;
        delete exec;
    }

    db->close();
    delete db;

    cout << "\nPronto!" << endl;

    return 0;
}
