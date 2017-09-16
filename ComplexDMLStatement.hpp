#ifndef COMPLEXDMLSTATEMENT_HPP
#define COMPLEXDMLSTATEMENT_HPP

#include <RegularSQLStatement.hpp>
#include <Syntactic.hpp>
#include <Semantic.hpp>
#include <fstream>

#include <QDebug>

using namespace std;

class ComplexDMLStatement : public RegularSQLStatement{


private:
    Syntactic *syntactic;
    vector<string> commandList;
    vector<string> ciaList;              //Stores the current list of image attributes
    vector<string> cpaList;              //Stores the current particulate attribute
    vector<string> caaList;              //Stores the current audio attribute
    vector<string> complexAttributeList; //Stores the list of complex attributes.
                                         //It is used to avoid the association of the same
                                         //metric more than one time for each image attribute.
                                         //It is used to avoid the insertion of the same
                                         //parameter more than one time in each IPV$ table.
    vector<string> metricReferenceList;  //Stores the list of metrics associated with a complex attribute.
                                         //It is used - in conjunction with the ComplexAttributeList - to
                                         //avoid the association of the metric more than one time for each
                                         //image attribute.
                                         //It is used - in conjunction with the ComplexAttributeList - to
                                         //avoid the insertion of the same parameter more than one time in
                                         //each IPV$ table.
    vector<string> metricList;           //Stores the list of metrics for each image attribute. It is
                                         //needed for processing the metric table constraint.
    vector<string> defaultMetricList;    //Stores list of Image Attributes that already a default metric
                                         //It is used to avoid the duplicity of default metrics for each
                                         //image attribute
    vector<string> particleColName;      //Stores the list of particle column names. It is used for handling
                                         //the particulate data type.
    vector<string> particleColType;      //Stores the list of particle column types. It is used for handling
                                         //the particulate data type.
    vector<string> particulateParam;     //Stores the list of particulate parameters. It is used for
                                         //handling the particulate data type.
    string currentColDataType;           //Stores the current column data type (stillimage or particulate)
    string currentMetric;                //Stores the current list of metrics
    vector<string> imgAttribList;        //Stores the image attribute list
    vector<string> audAttribList;        //Stores the audio attribute list
    vector<string> insertColList;        //Stores the insert column list
    vector<string> insertValueList;      //Stores the insert value list
    vector<string> imgAttribPosList;     //Stores a list of the image attribute position
    vector<string> audAttribPosList;     //Stores a list of the audio attribute position
    string attribPosition;               //Stores the current image/audio attribute position (used for
                                         //attributes that require feature extraction)
    vector<string> imgInsertAuxList;     //Stores a list of parameters that are used in the insert
                                         //commands used to alter IPV
    vector<string> audInsertAuxList;     //Stores a list of parameters that are used in the insert
                                         //commands used to alter APV

    private:
        bool insertParticulate(string tableName, string particulateId, vector<string> inserColList, vector<string> insertValueList);
        int chooseDMLStatement();

        vector<string> complexInsertIntoStatement();                    // insert_statement()
        vector<string> complexSelectStatement();                        // sql_manipulation_statement()

        //Semantic methods for SELECT ... FROM
        vector<Token> rebuildComplexSelectToRegularSelectStatement();  // query_specification()
        vector<Token> rebuildSelectKnnOrRangeToSelectInList();
        vector<Token> rebuildSelectComplexColumnsToSelectRegularColumns(vector<Token> tokenList, int scope);
        vector<Token> rebuildSelectAsteriskToSelectColumns(vector<Token> tokenList, int scope);
        vector<Token> performKnnOrRangeQuery (vector<Token> tokenList, int *activeListTail, int fileQueryObject, string predicate, string tableName, string attribName, string sgType, string metricName, string knn, string range, bool tieList, BasicArrayObject<double> **obj, int numberOfObjects);
        vector<Token> performKnnOrRangeJoinQuery(vector<Token> activeListTail, int *activeList, string predicate, string tableName, string tableName2,string attribName, string attribName2, string sgType, string metricName, string knn, string range, bool tieList, int *tempJoinTableScope);

    public:
        static const int COMPLEX_INSERT_INTO_STATEMENT       = 1;
        static const int COMPLEX_SELECT_STATEMENT            = 2;
        static const int COMPLEX_DELETE_STATEMENT            = 3;
        static const int COMPLEX_UPDATE_STATEMENT            = 4;

    public:
        ComplexDMLStatement(string sql, DatabaseManager *dbmg) : RegularSQLStatement(sql, dbmg){
            syntactic = new Syntactic(getDictionary());
        }

        vector<string> translateComplexSQLToRegularSQL();

        //Syntatic methods for INSERT INTO
        bool checkIfInsertValueIsValid(int attribPosition);      // insert_value()
        bool buildInsertValueList(int *attribPosition);          // insert_value_list()
        bool checkIfInsertColumnIsValid();                       // insert_column()
        bool buildColumnList();                                  // insert_column_list()

        //Syntatic methods for SELECT ... FROM
        bool checkIfFarPredicateIsValid();                       // far_predicate()
        bool checkIfTieListIsValid();                            // tie_list()
        bool checkIfKNearestValueIsValid();                      // k_nearest()
        bool checkIfRangeValueIsValid();                         // val_range()
        bool checkIfMetricNameIsValid();                         // metric_name()
        bool checkIfAssociedParameterValueListIsValid();         // parameter_val_assoc()
        bool buildAssociedParameterValueList();                  // parameter_val_assoc_list()
        bool checkIfQueryObjectDefinitionIsValid();              // query_obj_definition()
        bool checkIfNearPredicateIsValid();                      // near_predicate()
        bool checkIfValueExpressionIsValid();                    // value_expression()
        bool checkIfPredicateIsValid();                          // predicate()
        bool checkIfSearchConditionIsValid();                    // search_condition()
        bool checkIfHavingClauseIsValid();                       // having_clause()
        bool checkIfGroupByClauseIsValid();                      // groupby_clause()
        bool checkIfWhereClauseIsValid();                        // where_clause()
        bool buildTableReferenceList();                          // table_reference_list()
        bool checkIfFromClauseIsValid();                         // from_clause()
        bool checkIfTableExpressionIsValid();                    // table_expression()
        bool checkIfAsClauseIsValid();                           // as_clause()
        bool checkIfColumnRefenceIsValid();                      // column_reference()
        bool buildColumnReferenceList();                         // column_reference_list()
        bool buildSelectList();                                  // select_list()


        //Auxiliar methods for DML
        bool hasComplexAttributeInTheStatement(vector<Token> tokenList);
        bool insertImageIPV(string tableName, string stillImageName, string imageFile, string imageId);
        bool insertAudioAPV(string tableName, string audioImageName, string audioFile, string audioId);



};

#endif // COMPLEXDMLSTATEMENT_HPP
