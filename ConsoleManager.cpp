#include "ConsoleManager.hpp"

ConsoleManager::ConsoleManager(){

    last.clear();
    currentStatement().clear();
    name = "SiREn > ";
 }

ConsoleManager::~ConsoleManager(){

    statement.clear();
    last.clear();
    name.clear();
}

void ConsoleManager::printLogoLinux(){

    cout << endl;
    cout << " SIREN (R) Command Line Shell - January 2014 - Databases and Image Group" << endl;
    cout << "                     University of Sao Paulo -USP " << endl;
    cout << "      Developed under scholarship grants of FAPESP, CAPES and CNPq" << endl;
    cout << "        Distribution v2.0 by Marcos V N Bedo, William D Oliveira, " << endl;
    cout << "                Luiz Olmes and Gabriel de Pierro." << endl;
    cout << "     Supervisioned by Caetano Traina Jr., PhD and Agma J.Traina, PhD." << endl;
    cout << "                 See manual.txt for more information." << endl << endl << endl;
}

void ConsoleManager::printLogoWindows(){

    cout << endl;
    cout << " SIREN (R) Command Line Shell - January 2014 - Databases and Image Group" << endl;
    cout << "                     University of Sao Paulo -USP " << endl;
    cout << "      Developed under scholarship grants of FAPESP, CAPES and CNPq" << endl;
    cout << "        Distribution v2.0 by Marcos V N Bedo, William D Oliveira, " << endl;
    cout << "                Luiz Olmes and Gabriel de Pierro." << endl;
    cout << "     Supervisioned by Caetano Traina Jr., PhD and Agma J.Traina, PhD." << endl;
    cout << "                 See manual.txt for more information." << endl << endl << endl;
}

string ConsoleManager::backspace(string query){

    if (query.size() > 0){
        string aux;
        for (int x = 0; x < query.size()-1; x++){
            aux += query[x];
        }

        query.clear();
        query = aux;
        cout << '\b';
        cout << " ";
        cout << '\b';
        cout << '\r';
        cout << name << query;
        aux.clear();
    }

    return query;
}

int ConsoleManager::consoleManagerChar(){

    int ch;
    #ifdef __unix__
        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~( ICANON | ECHO );
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    #else
        ch = getch();
    #endif

        return ch;
}

int ConsoleManager::upDownArrowOrRegularChar(){

    int answer;

    int k = 0;
    int ch = consoleManagerChar();

    if (ch == 27){
        k = 1;
        ch = consoleManagerChar();
        if ((ch == 91) && (k == 1)){
            k = 2;
            ch = consoleManagerChar();
            if((ch == 65) && (k == 2)){
                answer = -500;
            } else {
                if((ch == 66) && (k == 2)){
                    answer = -501;
                } else {
                    if((ch == 68) && (k == 2)){
                        answer = -502;//left
                    } else {
                        if((ch == 67) && (k == 2)){
                            answer = -503;//right
                        } else {
                            answer = ch;
                        }
                    }
                }
            }
        } else {
            answer = ch;
        }
    } else {
        answer = ch;
    }

    return answer;
}

void ConsoleManager::printLogo(){

    #ifdef __unix__
        printLogoLinux();
    #else
        printLogoWindows();
    #endif
}

void ConsoleManager::clear(){

     #ifdef __unix__
        system("clear");
     #else
        system("cls");
     #endif
}

void ConsoleManager::close(){

    cout << "Bye!" << endl;
    exit(0);
}

void ConsoleManager::commit(DatabaseManager *db){

    db->runInsertStatement("commit");
    return;
}

string ConsoleManager::readStatement(){

    string query;

    statement.clear();
    query.clear();

    cout << name;

    int ch = upDownArrowOrRegularChar();

    while (!((((char) ch) == '\n') && (query.size() > 0) && (query[query.size()-1] == ';'))){

        if (ch == -500){
            if (query.size() > 0){
                for (int x = 0; x < query.size(); x++){
                    for (int x = 0; x < query.size(); x++){
                        cout << '\b';
                        cout << " ";
                        cout << '\b';
                    }
                 }
            }
            query.clear();
            cout << '\r';
            query = last;
            cout << name << query;
        } else {
            if (ch == -501){
                if (query.size() > 0){
                    for (int x = 0; x < query.size(); x++){
                        cout << '\b';
                        cout << " ";
                        cout << '\b';
                    }
                }
                cout << '\r';
                query.clear();
                cout << name << query;
            } else {
                if ((ch == 8) || (ch == 127)){
                    query = backspace(query);
                } else {
                    if (ch == 10){
                        cout << (char) ch;
                        cout << setw(name.size()) << "-> ";
                    } else {
                        query += ch;
                        cout << (char) ch;
                    }
                }
            }
        }
        ch = upDownArrowOrRegularChar();
    }

    for (int x = 0; x < query.size()-1; x++){
        if (query[x] == '\n'){
            statement += ' ';
        } else {
            statement += query[x];
        }
    }

    if (statement.size() > 0){
        last.clear();
        last = statement;
    }

    query.clear();

    cout << endl;

    return statement;
}

string ConsoleManager::currentStatement(){

    return statement;
}

string ConsoleManager::lastInputStatement(){

    return last;
}

void ConsoleManager::printResultQuery(DatabaseManager *db, vector<string> query, bool see){

    SirenTime *tme = new SirenTime();

    bool all = true;

    if (see){
        cout << "\033[1;36m" << "\nRegular SQL equivalent expressions:\033[0m \n" << endl;
    }
    for (int x = 0; x < query.size(); x++){

        if (!db->runInsertStatement(query[x])){
            all = false;
            cout << "\033[1;31m" << "Operation cannot be performed: \033[0m" << endl;
            cout << "\033[0;31m" << query[x] << "\033[0m \n" << endl;
        }
        else if (see) {
            cout << query[x] << "\n" << endl;
        }
    }

    if (all) {
        cout << "\033[1;32m" << "Operation successfully performed taking ";
        cout << tme->stopWatchInMilliseconds() << " ms. \033[0m \n" << endl;
    }


    delete (tme);

}

void ConsoleManager::printResultQuery(DatabaseManager *db, string query, bool see){

    SirenTime *tme = new SirenTime();

    vector< vector < string > > answer = db->runSelect(query);

    if (see){
        cout << "\033[1;36m" << "\nRegular SQL equivalent expressions:\033[0m \n" << endl;
        cout << query << "\n" << endl;
    }

    if (answer.size() > 0){
        vector< string > columns = db->getFieldNames(query);
        vector< int > maxSizes;
        string bar;

        for (int x = 0; x < columns.size(); x++){
            maxSizes.push_back(columns[x].size());
        }

        for (int x = 0; x < answer.size(); x++){
            for (int y = 0; y < answer[x].size(); y++){
                if (answer[x][y].size() > maxSizes[y]){
                    maxSizes[y] = answer[x][y].size();
                }
            }
        }

        for (int x = 0; x < maxSizes.size(); x++){
            bar += "+-";
            for (int y = 0; y < maxSizes[x]; y++){
                bar += "-";
            }
            bar += "-";
        }
        bar += "+";

        cout << bar << endl;
        cout << "| ";
        for (int x = 0; x < maxSizes.size(); x++){
            cout << setw(maxSizes[x]) << columns[x] << " | ";
        }
        cout << endl;
        cout << bar << endl;

        for (int x = 0; x < answer.size(); x++){
            cout << "| ";
            for (int y = 0; y < answer[x].size(); y++){
                cout << setw(maxSizes[y]) << answer[x][y] << " | ";
            }
            cout << endl;
        }
        cout << bar << endl;

    } else {
        cout << "Empty result set." << endl;
    }

    cout << "\n" << "\033[1;32m" << answer.size() << " rows as answer taking ";
    cout << tme->stopWatchInMilliseconds() << " ms. \033[0m \n" << endl;

    delete (tme);
}

void ConsoleManager::printError(vector<string> errors){

    cout << errors.size() << " error(s) found: " << endl;
    for (int x = 0; x < errors.size(); x++){
        cout << errors[x] << endl;
    }
}

void ConsoleManager::printParseTree(ParseTree *pt){

    cout << "Equivalent Parse Tree: " << endl;
    cout << pt->toString() << endl;
}

