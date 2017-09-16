#ifndef CONSOLEMANAGER_HPP
#define CONSOLEMANAGER_HPP

#ifdef __unix__
#include <termios.h>
#elif defined(_WIN32) || defined(WIN32)
#include <windows.h>
#include <conio.h>
#endif

#include <iostream>
#include <cstring>
#include <iomanip>
#include <DatabaseManager.hpp>
#include <SirenTime.hpp>
#include <ParseTreeFactory.hpp>

#include <unistd.h>

using namespace std;

class ConsoleManager{

    private:
        string statement;
        string last;
        string name;

    private:
        int consoleManagerChar();

        void printLogoLinux();
        void printLogoWindows();

        string backspace(string query);

    public:
        ConsoleManager();
        ~ConsoleManager();

        int upDownArrowOrRegularChar();

        void printLogo();
        void printResultQuery(DatabaseManager *db, string query, bool see = false);
        void printResultQuery(DatabaseManager *db, vector<string> query, bool see = false);
        void printError(vector<string> errors);
        void printParseTree(ParseTree *pt);

        void clear();
        void close();
        void commit(DatabaseManager *db);

        string readStatement();

        string currentStatement();
        string lastInputStatement();
};

#endif // CONSOLEMANAGER_HPP
