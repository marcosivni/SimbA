#ifndef DATABASEMANAGER_HPP
#define DATABASEMANAGER_HPP

#include <QtSql/QSqlQuery>
#include <QVariant>
#include <QByteArray>
#include <QSqlError>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlRecord>
#include <cmath>
#include <algorithm>
#include <vector>

using namespace std;

class DatabaseManager{

    private:
        QSqlDatabase db;
        string username;
        string password;
        string port;
        string ip;
        int sgbd;

    public:
        const static int ORACLE = 1;
        const static int POSTGRES = 2;
        const static int MYSQL = 3;

    private:
        string clearGarbageBlob(QByteArray array);

    public:
        DatabaseManager();
        DatabaseManager(string username, string password, string ip = "localhost", int sgbd = DatabaseManager::MYSQL, string port = "3306", string databaseName = "SIREN");
        ~DatabaseManager();

        void setUsername(string username);
        void setPassword(string password);
        void setPort(string port);
        void setIp(string ip);
        void setSGBD(int sgbd);

        bool isValid();

        int getSGBD();
        string getIp();
        string getUsername();
        string getPort();
        string getDataBaseName();

        vector< vector<string> > runSelect(string query);
        vector<string> getFieldNames(string query);
        int getNumberOfPossibleTuples(string query);
        string getEntireBlob(string queryId);
        bool runInsertStatement(string query);

        void close();
};

#endif // DATABASEMANAGER_HPP
