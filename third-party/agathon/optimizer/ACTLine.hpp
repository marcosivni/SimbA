#ifndef ACTLINE_HPP
#define ACTLINE_HPP

#include <iostream>
#include <cstring>
#include <map>

using namespace std;

class ACTLine{

    private:
        string attribute;
        int attributeType;
        string operattor;
        string value;
        int valueType;
        map < string, string > parameters;

    public:
        // Attribute type
        static const int ATTRIBUTE_TRADITIONAL = 0; // traditional
        static const int ATTRIBUTE_PARTICULATE = 1; // particulate... hum...
        static const int ATTRIBUTE_MONOLITHIC = 2;  // monolithic

        // Value type
        static const int VT_ATTR = 0; // attribute
        static const int VT_OPER = 1; // operator
        static const int VT_TREE = 2; // parse tree

    public:
        ACTLine();
        ~ACTLine();

        void setAttribute(string attribute);
        void setAttributeType(int attributeType);
        void setOperator(string operattor);
        void setValue(string value);
        void setValueType(int valueType);
        void setParameter(string key, string value);

        string getAttribute();
        int getAttributeType();
        string getOperator();
        string getValue();
        int getValueType();
        string getParameter(string key);

        //Generic methods
        bool isEqual(ACTLine *actLine);

        void clearParameter();
        string parameterToString();
};

#endif // ACTLINE_HPP
