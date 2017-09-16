#ifndef PARSER_HPP
#define PARSER_HPP

#include <Token.hpp>
#include <Lexical.hpp>
#include <vector>
#include <algorithm>


using namespace std;

class Parser{

    private:
        vector<Token*> list;
        string original;
        int position;
        int activeScope;

    private:
        void setPosition(int position);
        int getPosition();

    public:
        Parser();
        Parser(string input);
        ~Parser();

        void parse(string input);
        void resetParser();
        bool match(string identifier, int tokenType) throw (std::exception *);
        bool match(Token *token) throw (std::exception *);
        void insertToken(Token *token, int position) throw (std::exception *);
        void removeToken(int position) throw (std::exception *);
        bool updateToken(Token *token, int position);
        void insertTokenList(vector<Token *> newList, int position) throw (std::exception *);

        void defineRightScopes();

        int countTokens();
        void setActiveScope(int activeScope);

        int getActiveScope();
        string getOriginal();
        Token* getCurrentToken() throw (std::exception *);
        Token* getToken() throw (std::exception *);
        Token* getToken(int position) throw (std::exception *);
        vector<Token*> getTokenList();
        vector<Token> getTokenList(int scope);
        int getCurrentPosition();
        int getNumberOfScopes();

};

#endif // PARSER_HPP
