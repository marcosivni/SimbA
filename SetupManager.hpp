#ifndef SETUPMANAGER_HPP
#define SETUPMANAGER_HPP

#ifdef __unix__
#include <termios.h>
#elif defined(_WIN32) || defined(WIN32)
#include <windows.h>
#include <conio.h>
#endif

#include <ConsoleManager.hpp>

class SetupManager : public ConsoleManager{

    private:
        string user, password, ip, port;
        int dbms;
        bool pParseTree;
        bool pTranslatedRegular;

    public:
        SetupManager(int argc, char* argv[]);

        DatabaseManager *newDBMSInstance();

        bool printParseTree();
        bool printTranslatedSQL();
};


#endif // SETUPMANAGER_HPP
