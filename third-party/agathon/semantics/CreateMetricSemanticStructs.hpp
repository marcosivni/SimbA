#ifndef CREATEMETRICSEMANTICSTRUCTS_HPP
#define CREATEMETRICSEMANTICSTRUCTS_HPP

#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstring>

using namespace std;

class CheckExtractor{

    private:
        vector<pair <string, string> > extractorList;
        vector<pair <string, string> > parametersList;
        vector<string> weigth;
        string dataType;
        string error;

    public:
        CheckExtractor();
        ~CheckExtractor();

        void addWeigth(string w);
        void addParameter(string extractorName, string parameterName);
        void addExtractor(string extractorName, string extractorAlias = "");
        void addExtractorAlias(string extractorAlias);
        void addError(string error);
        void setDataType(string dataType);

        bool isValid();

        pair <string, string> getExtractor(int pos) throw (std::exception*);
        pair <string, string> getParameter(int pos) throw (std::exception*);
        string getWeigth(int pos) throw (std::exception*);
        int getSize();
        int getParametersSize();
        string getDataType();
        string getError();

        string toString();
};

#endif // CREATEMETRICSEMANTICSTRUCTS_HPP
