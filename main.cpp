//Internal includes
#include <SetupManager.hpp>
#include <ConsoleManager.hpp>
#include <DatabaseManager.hpp>

//Agathon includes
#include <ComplexSQLExecutor.hpp>
#include <SyntacticComplexSQLAnalyzer.hpp>

using namespace std;

int main(int argc, char* argv[]){

    ConsoleManager console;
    ParseTreeFactory factory;

    string input;
    SetupManager *options = NULL;

    //Set argc, argv
    bool login = false;
    while (!login){
        if (options != NULL){
            delete (options);
        }
        try{
            options = new SetupManager(argc, argv);
            login = true;
        } catch (std::string *tp){
            delete (tp);
            return 0;
        } catch (...){
            argc = 1;
            cout << "Invalid data to database connection. Try again." << endl;
        }
    }

    try{
        DatabaseManager *db = options->newDBMSInstance();

        console.printLogo();

        while (1){
            input.clear();
            input = console.readStatement();

            Parser *parser = new Parser(input);

            if ((parser->countTokens() == 1) && (parser->getToken(0)->toUpperLexem() == "CLEAR")){
                console.clear();
            } else {
                if ((parser->countTokens() == 1) && (parser->getToken(0)->toUpperLexem() == "EXIT")){
                    console.commit(db);
                    delete (db);
                    console.close();
                    delete (options);
                    return 0;
                } else {
                    SyntacticComplexSQLAnalyzer *analyzer = new SyntacticComplexSQLAnalyzer(input, db);
                    if (analyzer->isValid()){

                        ComplexSQLExecutor *executor = new ComplexSQLExecutor(input, db);
                        vector<string> sql = executor->translate();

                        if ((analyzer->isSelect()) || (analyzer->isDCL())){
                            if ((analyzer->isSelect()) && (options->printParseTree())){
                                console.printParseTree(factory.canonicalParseTree(input));
                            }
                            for (int k = 0; k < sql.size(); k++){
                                console.printResultQuery(db, sql[k], options->printTranslatedSQL());
                            }
                        } else {
                            console.printResultQuery(db, sql, options->printTranslatedSQL());
                        }
                        delete (executor);
                    } else {
                        console.printError(analyzer->getErrors());
                    }
                    delete (analyzer);

                }
            }

            delete (parser);

        }
    } catch (...) {
        cout << "Invalid parameters specification for DBMS connection. Try again." << endl;
        delete (options);
    }

    return 0;
}




