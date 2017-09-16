#include "DataDictionary.hpp"

DataDictionary::DataDictionary(DatabaseManager *dbmg)
{
    setDatabaseManager(dbmg);
}

DataDictionary::~DataDictionary()
{

}

string DataDictionary::unSlugify(string text)
{
    string answer;

    for(int i = 0; i < text.length(); i++)
    {
        if(text[i] == '\\') answer.append("\\\\");
        else answer += text[i];
    }

    return answer;
}

bool DataDictionary::isMetrical(string distanceCode)
{
    string strSql = "";

    if(getDBMS() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT UPPER(DistanceType) FROM EPD$DistanceFunctions WHERE DistanceCode ='" + distanceCode + "'";
    }
    else if(getDBMS() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT UPPER(DistanceType) FROM \"EPD$DistanceFunctions\" t WHERE t.DistanceCode ='" + distanceCode + "'";
    }
    else if(getDBMS() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    return ((getDatabaseManager()->runSelect(strSql)[0][0] == "METRICAL"));
}

bool DataDictionary::isPrimaryKey(string tableName, string columnName)
{
    string strSql = "";

    if(isValidColumn(tableName, columnName))
    {
        if(getDBMS() == DatabaseManager::MYSQL)
        {
            strSql = "SELECT k.column_name FROM information_schema.table_constraints t JOIN information_schema.key_column_usage k USING(constraint_name,table_schema,table_name) WHERE t.constraint_type='PRIMARY KEY' AND t.table_schema='" + getDatabaseManager()->getDataBaseName() + "' AND t.table_name='"+ tableName +"' AND k.column_name IN ('" + columnName + "')";
        }
        else if(getDBMS() == DatabaseManager::POSTGRES)
        {
            strSql = "SELECT k.column_name FROM information_schema.table_constraints t JOIN information_schema.key_column_usage k USING(constraint_name,table_catalog,table_name) WHERE t.constraint_type='PRIMARY KEY' AND t.table_catalog='" + getDatabaseManager()->getDataBaseName() + "' AND t.table_name='"+ tableName +"' AND k.column_name IN ('" + columnName + "')";
        }
        else if(getDBMS() == DatabaseManager::ORACLE)
        {
            // @todo
        }

        return getDatabaseManager()->runSelect(strSql).size() > 0;
    }

    return false;
}

bool DataDictionary::isValidPrimaryKey(string value, string tableName, string columnName)
{
    string strSql = "";
    vector< vector <string> > aux;

    if(isPrimaryKey(tableName, columnName))
    {
        if(getDBMS() == DatabaseManager::MYSQL)
        {
            strSql = "SELECT " + columnName + " FROM " + tableName;
        }
        else if(getDBMS() == DatabaseManager::POSTGRES)
        {
            strSql = "SELECT " + columnName + " FROM \"" + tableName + "\"";
        }
        else if(getDBMS() == DatabaseManager::ORACLE)
        {
            // @todo
        }

        aux = (getDatabaseManager()->runSelect(strSql));

        for (int x = 0; x < aux.size(); x++)
        {
            string auxs = "'";
            auxs.append(aux[x][0]);
            auxs.append("'");
            if ((aux[x][0] == value) || (auxs == value)){
                return false;
            }
            aux[x][0].clear();
            aux[x].clear();
        }

        aux.clear();
        return true;
    }

    return false;
}

bool DataDictionary::isValidDatabase(string databaseName)
{
    string strSql = "";

    if(getDBMS() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT DISTINCT TABLE_SCHEMA FROM information_schema.TABLES WHERE UPPER(TABLE_SCHEMA) = UPPER('" + databaseName + "')";
    }
    else if(getDBMS() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT DISTINCT t.table_catalog FROM information_schema.tables t WHERE UPPER(t.table_catalog) = UPPER('" + databaseName + "')";
    }
    else if(getDBMS() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    return (getDatabaseManager()->runSelect(strSql).size() > 0);
}

bool DataDictionary::isValidDataType(string dataType)
{
    string strSql1 = "";
    string strSql2 = "";

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql1 = "SELECT UPPER('" + dataType + "')";
        strSql2 = "SELECT DISTINCT UPPER(DATA_TYPE) FROM information_schema.COLUMNS WHERE UPPER(DATA_TYPE)='" + dataType + "' UNION SELECT DISTINCT(UPPER(ExtractorType)) FROM EPD$Extractors WHERE UPPER(ExtractorType)='"+ dataType +"'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql1 = "SELECT UPPER('" + dataType + "')";
        strSql2 = "SELECT DISTINCT UPPER(t1.data_type) FROM information_schema.columns t1 WHERE UPPER(t1.data_type) = UPPER('" + dataType + "') UNION SELECT DISTINCT(UPPER(t2.ExtractorType)) FROM \"EPD$Extractors\" t2 WHERE UPPER(t2.ExtractorType) = UPPER('"+ dataType +"')";
    }
    else if(getDBMS() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    if(getDatabaseManager()->runSelect(strSql1)[0][0] == "INTEGER")
    {
        return true;
    }
    else
    {
        return ((dataType == "INTEGER") || (getDatabaseManager()->runSelect(strSql2).size() > 0));
    }
}

bool DataDictionary::isValidComplexDataType(string dataType)
{
    string strSql = "";

    if (getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT DISTINCT(UPPER(ExtractorType)) FROM EPD$Extractors WHERE UPPER(ExtractorType)='"+ dataType +"'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT DISTINCT(UPPER(t.ExtractorType)) FROM \"EPD$Extractors\" t WHERE UPPER(t.ExtractorType)='"+ dataType +"'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    return (getDatabaseManager()->runSelect(strSql).size() > 0);
}

bool DataDictionary::isValidIndex(string tableName, string indexName)
{
    string strSql = "";

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT INDEX_TYPE FROM information_schema.statistics WHERE TABLE_SCHEMA='" + getDatabaseManager()->getDataBaseName() + "' AND TABLE_NAME='"+ tableName +"' AND INDEX_NAME='" + indexName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        // @todo - nao achei no catalogo do postgres :-(
        return true;
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    return (getDatabaseManager()->runSelect(strSql).size() > 0);
}

// Add features from other databases
bool DataDictionary::isValidTable(string tableName)
{
    string strSql = "";

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT ENGINE FROM information_schema.TABLES WHERE TABLE_SCHEMA='" + getDatabaseManager()->getDataBaseName() + "' AND TABLE_NAME='"+ tableName +"'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT t.table_name FROM information_schema.tables t WHERE t.table_catalog = '" + getDatabaseManager()->getDataBaseName() + "' AND t.table_name='"+ tableName +"'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    return (getDatabaseManager()->runSelect(strSql).size() > 0);
}

bool DataDictionary::isValidColumn(string tableName, string columnName)
{
    string strSql = "";

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT COLUMN_NAME FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='"+ getDatabaseManager()->getDataBaseName() + "' AND TABLE_NAME='" + tableName + "' AND COLUMN_NAME='" + columnName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT t.column_name FROM information_schema.columns t WHERE t.table_catalog = '" + getDatabaseManager()->getDataBaseName() + "' AND t.table_name='" + tableName + "' AND t.column_name='" + columnName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    return (getDatabaseManager()->runSelect(strSql).size() > 0);
}

bool DataDictionary::isValidColumn(vector<string> tableName, vector<string> columnName){

    if (getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL){
        string query = "SELECT COLUMN_NAME FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='"+ getDatabaseManager()->getDataBaseName() + "' AND TABLE_NAME IN ( ";

        for (int x = 0; x < tableName.size(); x++){
            if (x > 0)
                query += ", ";
            query += "'" + tableName[x] + "'";
        }

        query += ") AND COLUMN_NAME IN (";

        for (int x = 0; x < columnName.size(); x++){
            if (x > 0)
                query += ", ";
            query += "'" + columnName[x] + "'";
        }

        query += ")";

        return (getDatabaseManager()->runSelect(query).size() > 0);
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        string query = "SELECT t.column_name FROM information_schema.columns t WHERE t.table_catalog = '" + getDatabaseManager()->getDataBaseName() + "' AND t.table_name IN ( ";

        for (int x = 0; x < tableName.size(); x++)
        {
            if (x > 0)
                query += ", ";
            query += "'" + tableName[x] + "'";
        }

        query += ") AND t.column_name IN (";

        for (int x = 0; x < columnName.size(); x++)
        {
            if (x > 0)
                query += ", ";
            query += "'" + columnName[x] + "'";
        }

        query += ")";

        return (getDatabaseManager()->runSelect(query).size() > 0);
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
        return true;
    }
}

string DataDictionary::toUpper(string text)
{
    return getDatabaseManager()->runSelect("SELECT UPPER('" + text + "')")[0][0];
}

string DataDictionary::dropIndexStatement(string table, string indexName)
{
    if(getDBMS() ==  DatabaseManager::MYSQL)
    {
        return "ALTER TABLE " + table + " DROP INDEX " + indexName;
    }
    else if(getDBMS() == DatabaseManager::POSTGRES)
    {
        return "DROP INDEX " + indexName;
    }
    else if(getDBMS() == DatabaseManager::ORACLE)
    {
        return "DROP INDEX " + indexName;
    }
}

string DataDictionary::useStatement(string databaseName)
{
    if(getDatabaseManager()->getSGBD() ==  DatabaseManager::MYSQL)
    {
        return "USE " + databaseName;
    }

    return "NULL";
}

string DataDictionary::getNewIndexId()
{
    int cid = runSequence("IndexIdSequence");
    char last[21];
    sprintf(last, "%d", cid);
    string tmp = last;

    return tmp;
}

string DataDictionary::getNewMetricId()
{
    int cid = runSequence("MetricIdSequence");
    char last[21];
    sprintf(last, "%d", cid);
    string tmp = last;

    return tmp;
}

vector<string> DataDictionary::getComplexReferenceId(string table, string column, string whereClause)
{
    vector< vector<string> > aux;
    vector<string> answer;
    string query = "";

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        query = "SELECT " + column + " FROM " + table;
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        query = "SELECT " + column + " FROM \"" + table + "\"";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    if (!whereClause.empty())
    {
        query += " WHERE " + whereClause;
    }

    aux = getDatabaseManager()->runSelect(query);
    if (aux.size() > 0)
    {
        for (int x = 0; x < aux.size(); x++)
        {
            answer.push_back(aux[x][0]);
        }

        for (int x = 0; x < aux.size(); x++)
        {
            for (int y = 0; y < aux[x].size(); y++)
            {
                aux[x][y].clear();
            }
            aux[x].clear();
        }
        aux.clear();
    }

    query.clear();
    return answer;
}

string DataDictionary::getTablePrefix(string complexData)
{
    string strSql = "";

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT UPPER(acronym) FROM CDD$Acronym WHERE UPPER(caType) = UPPER('" + complexData + "')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT UPPER(t.acronym) FROM \"CDD$Acronym\" t WHERE UPPER(t.caType) = UPPER('" + complexData + "')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    vector < vector < string > > result = getDatabaseManager()->runSelect(strSql);

    if(result.size() > 0)
    {
        return result[0][0];
    }
    else
    {
        return "";
    }
}

string DataDictionary::getColumnDataType(string tableName, string columnName)
{
    string strSql1 = "";
    string strSql2 = "";

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql1 = "SELECT UPPER(ComplexAttribType) FROM CDD$ComplexAttribute WHERE TableName='" + tableName + "' AND ComplexAttribName = '" + columnName + "'";
        strSql2 = "SELECT UPPER(DATA_TYPE) FROM information_schema.COLUMNS WHERE TABLE_NAME='" + tableName +"' AND COLUMN_NAME='" + columnName + "' AND TABLE_SCHEMA='" + getDatabaseManager()->getDataBaseName() + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql1 = "SELECT UPPER(t1.ComplexAttribType) FROM \"CDD$ComplexAttribute\" t1 WHERE t1.TableName='" + tableName + "' AND t1.ComplexAttribName = '" + columnName + "'";
        strSql2 = "SELECT UPPER(t2.data_type) FROM information_schema.columns t2 WHERE t2.table_name = '" + tableName +"' AND t2.column_name= '" + columnName + "' AND t2.table_catalog= '" + getDatabaseManager()->getDataBaseName() + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    // @loc
    // Complex data type
    vector< vector <string> > aux = getDatabaseManager()->runSelect(strSql1);
    if (aux.size() > 0)
    {
        return aux[0][0];
    }

    // @loc
    // Traditional data type
    aux = getDatabaseManager()->runSelect(strSql2);
    if(aux.size() > 0)
    {
        if(aux[0][0] == "INT") return "INTEGER";
        else return aux[0][0];
    }

    return "NULL";
}

string DataDictionary::showTables()
{
    string answer = "";

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        answer = "SHOW TABLES";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        answer = "SELECT table_name FROM information_schema.tables WHERE table_schema = 'public'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    return answer;
}

string DataDictionary::showDatabases()
{
    string answer = "";

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        answer = "SHOW DATABASES";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        answer = "SELECT datname FROM pg_database";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    return answer;
}

string DataDictionary::getAllTraditionalIndexesStatement()
{
    string answer = "";

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        answer = "SELECT INDEX_NAME, TABLE_NAME, INDEX_TYPE FROM information_schema.statistics WHERE TABLE_SCHEMA='" + getDatabaseManager()->getDataBaseName() + "' UNION SELECT IndexName, TableName, 'SLIMTREE' FROM CDD$ComplexAttribMetric";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        answer = "SELECT t.IndexName, t.TableName, 'SLIMTREE' FROM \"CDD$ComplexAttribMetric\" t";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    return answer;
}

string DataDictionary::descMetricCommand(string metricName)
{
    string answer = "";

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        answer = "SELECT MetricName AS Metric, MetricType AS Type, ExtractorName AS Extractor, DistanceName AS DistanceFunction FROM CDD$MetricStruct m JOIN EPD$DistanceFunctions d ON m.DistanceCode = d.DistanceCode JOIN CDD$ParameterStruct p ON m.MetricCode=p.MetricCode JOIN EPD$Extractors e ON p.ExtractorCode=e.ExtractorCode WHERE MetricName='" + metricName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        answer = "SELECT m.MetricName AS Metric, m.MetricType AS Type, e.ExtractorName AS Extractor, d.DistanceName AS DistanceFunction FROM \"CDD$MetricStruct\" m JOIN \"EPD$DistanceFunctions\" d ON m.DistanceCode = d.DistanceCode JOIN \"CDD$ParameterStruct\" p ON m.MetricCode=p.MetricCode JOIN \"EPD$Extractors\" e ON p.ExtractorCode=e.ExtractorCode WHERE MetricName='" + metricName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    return answer;
}

string DataDictionary::descTableCommand(string tableName)
{
    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        return "DESC " + tableName;
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        return "SELECT column_name FROM information_schema.columns WHERE table_name ='"+ tableName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        return "";
    }
}

string DataDictionary::getDistanceName(string distanceCode)
{
    string strSql = "";

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT df.DistanceName FROM CDD$ComplexAttribute ca JOIN CDD$ComplexAttribMetric cm ON ca.TableName=cm.TableName AND ca.ComplexAttribName = cm.ComplexAttribName JOIN CDD$MetricStruct ms ON cm.MetricCode = ms.MetricCode JOIN EPD$DistanceFunctions df ON ms.DistanceCode = df.DistanceCode WHERE df.DistanceCode = " + distanceCode;
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT df.DistanceName FROM \"CDD$ComplexAttribute\" ca JOIN \"CDD$ComplexAttribMetric\" cm ON ca.TableName=cm.TableName AND ca.ComplexAttribName = cm.ComplexAttribName JOIN \"CDD$MetricStruct\" ms ON cm.MetricCode = ms.MetricCode JOIN \"EPD$DistanceFunctions\" df ON ms.DistanceCode = df.DistanceCode WHERE df.DistanceCode = " + distanceCode;
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    vector < vector < string > > result = getDatabaseManager()->runSelect(strSql);

    if (result.size() > 0)
    {
        return result[0][0];
    }
    else
    {
        return "NULL";
    }
}

vector<string> DataDictionary::getParametersName(string extractorCode)
{
    string strSql = "";
    vector<string> answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT ps.ParameterName FROM CDD$ComplexAttribMetric cam JOIN CDD$ComplexAttribute ca ON ca.TableName = cam.TableName AND ca.ComplexAttribName=cam.ComplexAttribName JOIN CDD$MetricStruct ms ON cam.MetricCode = ms.MetricCode JOIN CDD$ParameterStruct ps ON ms.MetricCode = ps.MetricCode JOIN EPD$Extractors ex ON ps.ExtractorCode = ex.ExtractorCode WHERE ex.ExtractorCode = '" + extractorCode + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT ps.ParameterName FROM \"CDD$ComplexAttribMetric\" cam JOIN \"CDD$ComplexAttribute\" ca ON ca.TableName = cam.TableName AND ca.ComplexAttribName=cam.ComplexAttribName JOIN \"CDD$MetricStruct\" ms ON cam.MetricCode = ms.MetricCode JOIN \"CDD$ParameterStruct\" ps ON ms.MetricCode = ps.MetricCode JOIN \"EPD$Extractors\" ex ON ps.ExtractorCode = ex.ExtractorCode WHERE ex.ExtractorCode = '" + extractorCode + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    vector< vector <string> > aux = getDatabaseManager()->runSelect(strSql);

    for(int x = 0; x < aux.size(); x++)
    {
        answer.push_back(aux[x][0]);
    }

    for(int x = 0; x < aux.size(); x++)
    {
        for(int y = 0; y < aux[x].size(); y++)
        {
            aux[x][y].clear();
        }
        aux[x].clear();
    }
    aux.clear();

    return answer;
}

vector<string> DataDictionary::getExtractorsName(string metricCode)
{
    string strSql = "";
    vector<string> answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT UPPER(ExtractorName) FROM CDD$ComplexAttribMetric cam JOIN CDD$ComplexAttribute ca ON ca.TableName = cam.TableName AND ca.ComplexAttribName=cam.ComplexAttribName JOIN CDD$MetricStruct ms ON cam.MetricCode = ms.MetricCode JOIN CDD$ParameterStruct ps ON ms.MetricCode = ps.MetricCode JOIN EPD$Extractors ex ON ex.ExtractorCode = ps.ExtractorCode WHERE  ms.MetricCode = '" +  metricCode + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT UPPER(ExtractorName) FROM \"CDD$ComplexAttribMetric\" cam JOIN \"CDD$ComplexAttribute\" ca ON ca.TableName = cam.TableName AND ca.ComplexAttribName=cam.ComplexAttribName JOIN \"CDD$MetricStruct\" ms ON cam.MetricCode = ms.MetricCode JOIN \"CDD$ParameterStruct\" ps ON ms.MetricCode = ps.MetricCode JOIN \"EPD$Extractors\" ex ON ex.ExtractorCode = ps.ExtractorCode WHERE  ms.MetricCode = '" +  metricCode + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    vector< vector <string> > aux = getDatabaseManager()->runSelect(strSql);

    for(int x = 0; x < aux.size(); x++)
    {
        answer.push_back(aux[x][0]);
    }

    for(int x = 0; x < aux.size(); x++)
    {
        for(int y = 0; y < aux[x].size(); y++)
        {
            aux[x][y].clear();
        }
        aux[x].clear();
    }
    aux.clear();

    return answer;
}

vector<string> DataDictionary::getAllColumnsFromTable(string tableName)
{
    string strSql = "";
    vector<string> answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT COLUMN_NAME FROM information_schema.COLUMNS WHERE TABLE_SCHEMA='"+ getDatabaseManager()->getDataBaseName() + "' AND TABLE_NAME='" + tableName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT t.column_name FROM information_schema.columns t WHERE t.table_catalog = '"+ getDatabaseManager()->getDataBaseName() + "' AND t.table_name='" + tableName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    vector< vector <string> > aux = getDatabaseManager()->runSelect(strSql);

    for(int x = 0; x < aux.size(); x++)
    {
        answer.push_back(aux[x][0]);
    }

    for(int x = 0; x < aux.size(); x++)
    {
        for(int y = 0; y < aux[x].size(); y++)
        {
            aux[x][y].clear();
        }
        aux[x].clear();
    }
    aux.clear();

    return answer;
}

bool DataDictionary::isDistanceFunctionName(string distanceName)
{
    string strSql = "";

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT DistanceCode FROM EPD$DistanceFunctions WHERE UPPER(DistanceName)=UPPER('" + distanceName + "')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT t.DistanceCode FROM \"EPD$DistanceFunctions\" t WHERE UPPER(t.DistanceName) = UPPER('" + distanceName +"')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    return getDatabaseManager()->runSelect(strSql).size() > 0;
}

bool DataDictionary::isExtractorName(string extractorName)
{
    string strSql = "";

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT * FROM EPD$Extractors WHERE UPPER(ExtractorName) = UPPER('" + extractorName + "')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT * FROM \"EPD$Extractors\" t WHERE UPPER(t.ExtractorName) = UPPER('" + extractorName + "')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    return getDatabaseManager()->runSelect(strSql).size() > 0;
}

bool DataDictionary::isValidExtractorForThisType(string dataType, string extractorName)
{
    string strSql = "";

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT * FROM EPD$Extractors WHERE UPPER(ExtractorType) = UPPER('" + dataType + "') AND UPPER(ExtractorName) = UPPER('" + extractorName + "')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT * FROM \"EPD$Extractors\" t WHERE UPPER(t.ExtractorType) = UPPER('" + dataType + "') AND UPPER(t.ExtractorName) = UPPER('" + extractorName + "')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    return getDatabaseManager()->runSelect(strSql).size() > 0;
}

bool DataDictionary::isExtractorParameter(int extractorCode, string parameterName)
{
    string strSql = "";
    char aux[8];
    sprintf(aux, "%d", extractorCode);
    string ec = aux;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT COUNT(ParameterName) AS AMOUNT FROM EPD$Parameters WHERE ExtractorCode = " + ec + " AND UPPER(ParameterName) = UPPER('" + parameterName + "')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT COUNT(t.ParameterName) AS AMOUNT FROM \"EPD$Parameters\" t WHERE t.ExtractorCode = " + ec + " AND UPPER(t.ParameterName) = UPPER('" + parameterName + "')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    vector < vector < string > > result = getDatabaseManager()->runSelect(strSql);

    if(result.size() > 0) return result[0][0] != "0";
    return false;
}

bool DataDictionary::isMetric(string metricName)
{
    string strSql = "";

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT MetricCode FROM CDD$MetricStruct WHERE MetricName = '" + metricName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT t.MetricCode FROM \"CDD$MetricStruct\" t WHERE t.MetricName = '" + metricName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    return getDatabaseManager()->runSelect(strSql).size() > 0;
}

bool DataDictionary::isComplexAttributeReference(string tableName, string attributeName)
{
    string strSql = "";

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT ComplexAttribType AS complextype FROM CDD$ComplexAttribute WHERE TableName = '" + tableName + "' AND ComplexAttribName = '" + attributeName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT t.ComplexAttribType AS complextype FROM \"CDD$ComplexAttribute\" t WHERE t.TableName = '" + tableName + "' AND t.ComplexAttribName = '" + attributeName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    return getDatabaseManager()->runSelect(strSql).size() > 0;
}

bool DataDictionary::isComplexAttributeReference(string attributeName)
{
    string strSql = "";

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT count(*) FROM CDD$ComplexAttribute WHERE ComplexAttribName = '" + attributeName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT count(*) FROM \"CDD$ComplexAttribute\" t WHERE t.ComplexAttribName = '" + attributeName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    vector < vector < string > > result = getDatabaseManager()->runSelect(strSql);

    if(result.size() > 0) return result[0][0] != "0";
    return false;
}

bool DataDictionary::isValidMetric(string metricName, string tableName, string attributeName)
{
    string strSql = "";

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT Count(*) FROM CDD$ComplexAttribMetric WHERE UPPER(TableName) = UPPER('" + tableName + "') AND UPPER(ComplexAttribName) = UPPER('" + attributeName + "') AND MetricCode IN (SELECT MetricCode FROM CDD$MetricStruct WHERE UPPER(MetricName) = UPPER('" + metricName + "'))";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT count(*) FROM \"CDD$ComplexAttribMetric\" t1 WHERE UPPER(t1.TableName) = UPPER('" + tableName + "') AND UPPER(t1.ComplexAttribName) = UPPER('" + attributeName + "') AND t1.MetricCode IN (SELECT t2.MetricCode FROM \"CDD$MetricStruct\" t2 WHERE UPPER(t2.MetricName) = UPPER('" + metricName + "'))";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    vector < vector < string > > result = getDatabaseManager()->runSelect(strSql);

    if(result.size() > 0) return result[0][0] != "0";
    return false;
}

bool DataDictionary::isComplexAttributeIndex(string caIndex)
{
    string strSql = "";

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT count(*) FROM CDD$ComplexAttribMetric WHERE IndexName = '" + caIndex + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT count(*) FROM \"CDD$ComplexAttribMetric\" t WHERE t.IndexName = '" + caIndex + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    vector < vector < string > > result = getDatabaseManager()->runSelect(strSql);

    if(result.size() > 0) return result[0][0] != "0";
    return false;
}

bool DataDictionary::isComplexAttributeIndex(string tableName, string caIndex)
{
    string strSql = "";

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT count(*) FROM CDD$ComplexAttribMetric WHERE IndexName = '" + caIndex + "' AND TableName='" + tableName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT count(*) FROM \"CDD$ComplexAttribMetric\" t WHERE t.IndexName = '" + caIndex + "' AND t.TableName = '" + tableName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    vector < vector < string > > result = getDatabaseManager()->runSelect(strSql);

    if(result.size() > 0) return result[0][0] != "0";
    return false;
}

bool DataDictionary::isStillimageAttributeReference(string attribName)
{
    string strSql = "";

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT Count(*) AS amount FROM CDD$ComplexAttribMetric WHERE UPPER(ComplexAttribName) = UPPER('" + attribName + "')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT count(*) AS amount FROM \"CDD$ComplexAttribMetric\" t where UPPER(t.ComplexAttribName) = UPPER('" + attribName + "')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    vector < vector < string > > result = getDatabaseManager()->runSelect(strSql);

    if(result.size() > 0) return result[0][0] != "0";
    return false;
}

bool DataDictionary::isStillimageAttributeReference(string tableName, string imgAttribName)
{
    string strSql = "";

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "select Count(*) as amount from CDD$ComplexAttribute where UPPER(TableName) = UPPER('" + tableName + "') AND UPPER(ComplexAttribName) = UPPER('" + imgAttribName + "') AND UPPER(ComplexAttribType) = UPPER('stillimage')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT count(*) AS amount FROM \"CDD$ComplexAttribute\" t WHERE UPPER(t.TableName) = UPPER('" + tableName + "') AND UPPER(t.ComplexAttribName) = UPPER('" + imgAttribName + "') AND UPPER(t.ComplexAttribType) = UPPER('stillimage')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    vector < vector < string > > result = getDatabaseManager()->runSelect(strSql);

    if(result.size() > 0) return result[0][0] != "0";
    return false;
}

bool DataDictionary::isParticulateMetric(string metricName)
{
    string strSql = "";

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "select UPPER(MetricType) from CDD$MetricStruct where MetricName = '" + metricName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT UPPER(t.MetricType) FROM \"CDD$MetricStruct\" t where t.MetricName = '" + metricName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    vector < vector < string > > result = getDatabaseManager()->runSelect(strSql);

    if(result.size() > 0) return result[0][0] == "PARTICULATE";
    return false;
}

bool DataDictionary::isComplexAttribMetricTuple(string tableName, string caAttribName, string metricCode)
{
    string strSql = "";

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT * FROM CDD$ComplexAttribMetricTuple WHERE UPPER(TableName) = UPPER('" + tableName + "') AND UPPER(ComplexAttribName) = UPPER('" + caAttribName + "') AND MetricCode='" + metricCode + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT * FROM \"CDD$ComplexAttribMetricTuple\" t WHERE UPPER(t.TableName) = UPPER('" + tableName + "') AND UPPER(t.ComplexAttribName) = UPPER('" + caAttribName + "') AND t.MetricCode = '" + metricCode + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    return getDatabaseManager()->runSelect(strSql).size() > 0;
}

int DataDictionary::runSequence(string sequenceName)
{
    string strSql1 = "";
    string strSql2 = "";
    string strSql3 = "";
    int result = -1;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql1 = "SELECT last_value FROM EPD$Sequences WHERE name = '" + sequenceName + "'";
        strSql2 = "INSERT INTO EPD$Sequences VALUES ( '" + sequenceName + "', '0' )";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql1 = "SELECT t.last_value FROM \"EPD$Sequences\" t WHERE t.name = '" + sequenceName + "'";
        strSql2 = "INSERT INTO \"EPD$Sequences\" VALUES ( '" + sequenceName + "', '0' )";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    vector < vector < string > > aux = getDatabaseManager()->runSelect(strSql1);

    if(aux.size() > 0)
    {
        result = atoi( aux[0][0].c_str() );
        result++;
        //criar run update
        char last[8];
        sprintf(last, "%d", result);
        string tmp = last;

        if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
        {
            strSql3 = "UPDATE EPD$Sequences SET last_value=" + tmp + " WHERE name = '" + sequenceName + "'";
        }
        else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
        {
            strSql3 = "UPDATE \"EPD$Sequences\" SET last_value = " + tmp + " WHERE name = '" + sequenceName + "'";
        }
        else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
        {
            // @todo
        }

        getDatabaseManager()->runSelect(strSql3);
    }
    else
    {
        getDatabaseManager()->runSelect(strSql2);
        result = 0;
    }

    return result;
}

vector< pair <string, string> > DataDictionary::getAllTableNameComplexColumn(string metricCode)
{
    string strSql = "";
    vector< pair <string, string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT DISTINCT TableName, ComplexAttribName FROM CDD$ComplexAttribMetric WHERE MetricCode='" + metricCode + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT DISTINCT t.TableName, t.ComplexAttribName FROM \"CDD$ComplexAttribMetric\" t WHERE t.MetricCode='" + metricCode + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    vector< vector<string> > aux = getDatabaseManager()->runSelect(strSql);

    for (int x = 0; x < aux.size(); x++)
    {
        pair <string, string> p (aux[x][0], aux[x][1]);
        answer.push_back(p);
    }

    return answer;
}

vector< pair <string, bool> > DataDictionary::getColumnsAndNullableFromTable(string tableName)
{
    string strSql = "";
    vector< pair <string, bool> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT COLUMN_NAME, IS_NULLABLE FROM information_schema.COLUMNS WHERE TABLE_NAME='" + tableName + "' AND TABLE_SCHEMA='" + getDatabaseManager()->getDataBaseName() + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT t.column_name, t.is_nullable FROM information_schema.columns t WHERE t.table_name = '" + tableName + "' AND t.table_catalog = '" + getDatabaseManager()->getDataBaseName() + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    vector< vector<string> > aux = getDatabaseManager()->runSelect(strSql);

    for(int x = 0; x < aux.size(); x++)
    {
        if (aux[x][1] == "NO")
        {
            pair <string, bool> p (aux[x][0], false);
            answer.push_back(p);
        }
        else
        {
            pair <string, bool> p (aux[x][0], true);
            answer.push_back(p);
        }
    }

    return answer;
}

vector<string> DataDictionary::getAllValidDataTypes()
{
    string strSql = "";
    vector<string> result;
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT DISTINCT(UPPER(ExtractorType)) FROM EPD$Extractors";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT DISTINCT(UPPER(t.ExtractorType)) FROM \"EPD$Extractors\" t";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = getDatabaseManager()->runSelect(strSql);

    if(answer.size() > 0)
    {
        for(int x = 0; x < answer.size(); x++)
        {
            result.push_back(answer[x][0]);
        }
    }

    for(int x = 0; x < answer.size(); x++)
    {
        for(int y = 0; y < answer[x].size(); y++)
        {
            answer[x][y].clear();
        }
        answer[x].clear();
    }
    answer.clear();

    return result;
}

vector< pair <string, string> > DataDictionary::getComplexObjectAndRowId(string tableName, string column)
{
    string strSql = "";
    vector< pair<string, string> > result;
    vector< vector<string> > answer;
    string id = column + "_id";

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT " + column + ", " + id + " FROM " + tableName;
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT " + column + ", " + id + " FROM \"" + tableName + "\"";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = getDatabaseManager()->runSelect(strSql);

    if(answer.size() > 0)
    {
        for(int x = 0; x < answer.size(); x++)
        {
            pair <string, string> aux;
            aux.first =  answer[x][0];
            aux.second =  answer[x][1];
            result.push_back(aux);
        }
    }

    for(int x = 0; x < answer.size(); x++)
    {
        for(int y = 0; y < answer[x].size(); y++)
        {
            answer[x][y].clear();
        }
        answer[x].clear();
    }
    answer.clear();

    return result;
}

string DataDictionary::getDefaultDistanceFunctionCode()
{
    string strSql = "";
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT MIN(DistanceCode) FROM EPD$DistanceFunctions";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT MIN(t.DistanceCode) FROM \"EPD$DistanceFunctions\" t";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = getDatabaseManager()->runSelect(strSql);

    if((answer.size() > 0) && (answer[0].size() > 0)) return answer[0][0];

    return "NULL";
}

string DataDictionary::getComplexDataTypeCode(string dataType)
{
    string strSql = "";
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT id FROM CDD$Acronym WHERE UPPER(caType) = UPPER('" + dataType +"')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT t.id FROM \"CDD$Acronym\" t WHERE UPPER(t.caType) = UPPER('" + dataType +"')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = getDatabaseManager()->runSelect(strSql);

    if((answer.size() > 0) && (answer[0].size() > 0)) return answer[0][0];

    return "NULL";
}

string DataDictionary::getDistanceFunctionCode(string distanceName)
{
    string strSql = "";
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT DistanceCode FROM EPD$DistanceFunctions WHERE UPPER(DistanceName)=UPPER('" + distanceName +"')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT t.DistanceCode FROM \"EPD$DistanceFunctions\" t WHERE UPPER(t.DistanceName)=UPPER('" + distanceName +"')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = getDatabaseManager()->runSelect(strSql);

    if((answer.size() > 0) && (answer[0].size() > 0)) return answer[0][0];

    return "NULL";
}

string DataDictionary::getExtractorCode(string extractorName)
{
    string strSql = "";
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT ExtractorCode FROM EPD$Extractors WHERE UPPER(ExtractorName) = UPPER('" + extractorName + "')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT t.ExtractorCode FROM \"EPD$Extractors\" t WHERE UPPER(t.ExtractorName) = UPPER('" + extractorName + "')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = getDatabaseManager()->runSelect(strSql);

    if((answer.size() > 0) && (answer[0].size() > 0)) return answer[0][0];

    return "NULL";
}

string DataDictionary::getMetricCode(string metricName)
{
    string strSql = "";
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT MetricCode FROM CDD$MetricStruct WHERE UPPER(MetricName) = UPPER('" + metricName + "')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT t.MetricCode FROM \"CDD$MetricStruct\" t WHERE UPPER(t.MetricName) = UPPER('" + metricName + "')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = getDatabaseManager()->runSelect(strSql);

    if((answer.size() > 0) && (answer[0].size() > 0)) return answer[0][0];

    return "NULL";
}

string DataDictionary::getComplexAttributeReference(string tableName, string attributeName)
{
    string strSql = "";
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT UPPER(ComplexAttribType) AS complextype FROM CDD$ComplexAttribute WHERE TableName = '" + tableName + "' AND ComplexAttribName = '" + attributeName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT UPPER(t.ComplexAttribType) AS complextype FROM \"CDD$ComplexAttribute\" t WHERE t.TableName = '" + tableName + "' AND t.ComplexAttribName = '" + attributeName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = getDatabaseManager()->runSelect(strSql);

    if((answer.size() > 0) && (answer[0].size() > 0)) return answer[0][0];

    return "NULL";
}

string DataDictionary::getDbmsDataType(string attribType){

    string s_query;

    /*if (getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
        s_query = "select tablename from pg_tables where tableowner = user and tablename = 'tmp$attribname'";
    else
        s_query = "select table_name from all_tables where owner = user and table_name = 'TMP$ATTRIBNAME'";

    vector< vector<string> > answer = getDatabaseManager()->runSelect(s_query.c_str());

    if (answer.size() > 0){
        if (answer[0][0] != ""){
            s_query = "drop table TMP$ATTRIBNAME";
            getDatabaseManager()->runSelect(s_query.c_str());
        }
    }

    s_query = "create table TMP$ATTRIBNAME (X " + attribType + ")";
    getDatabaseManager()->runSelect(s_query.c_str());


    if (getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
        s_query = "select data_type from pg_tables join information_schema.columns on table_name = tablename where tableowner = 'postgres' and table_name = 'tmp$attribname' and column_name = 'x'";
    else
        s_query = "select data_type from sys.all_tab_columns where owner = user and table_name = 'TMP$ATTRIBNAME' and column_name = 'X'";


    answer = getDatabaseManager()->runSelect(s_query.c_str());

    qDebug()  << s_query.c_str();
    //s_query = "drop table TMP$ATTRIBNAME";
    getDatabaseManager()->runSelect(s_query.c_str());

    if (answer.size() > 0)
        return answer[0][0];
    else
        return "";*/

    return attribType;
}

string DataDictionary::getDefaultMetric(string tableName, string attribName)
{
    string strSql = "";
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT ms.MetricName FROM CDD$ComplexAttribMetric im, CDD$MetricStruct ms WHERE im.metriccode = ms.metriccode and TableName = '" + tableName + "' and ComplexAttribName = '" + attribName + "' AND UPPER(IsDefault) = 'Y'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT ms.MetricName FROM \"CDD$ComplexAttribMetric\" im, \"CDD$MetricStruct\" ms WHERE im.MetricCode = ms.MetricCode AND im.TableName = '" + tableName + "' AND im.ComplexAttribName = '" + attribName + "' AND UPPER(im.IsDefault) = 'Y'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = getDatabaseManager()->runSelect(strSql);

    if((answer.size() > 0) && (answer[0].size() > 0)) return answer[0][0];

    return "NULL";
}

string DataDictionary::getMetricName(string metricCode)
{
    string strSql = "";
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT UPPER(MetricName) FROM CDD$MetricStruct WHERE MetricCode = " + metricCode;
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT UPPER(t.MetricName) FROM \"CDD$MetricStruct\" t WHERE t.MetricCode = " + metricCode;
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = getDatabaseManager()->runSelect(strSql);

    if((answer.size() > 0) && (answer[0].size() > 0)) return answer[0][0];

    return "NULL";
}

string DataDictionary::getMetricType(string metricCode)
{
    string strSql = "";
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT UPPER(MetricType) FROM CDD$MetricStruct WHERE MetricCode = " + metricCode;
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT UPPER(t.MetricType) FROM \"CDD$MetricStruct\" t WHERE t.MetricCode = " + metricCode;
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = getDatabaseManager()->runSelect(strSql);

    if((answer.size() > 0) && (answer[0].size() > 0)) return answer[0][0];

    return "NULL";
}

vector< pair<string, string> > DataDictionary::getMetrics(string tableName, string attribName)
{
    string strSql = "";
    pair<string, string> metricCodeAndComplexAttrib;
    vector< pair<string, string> > result;
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "select MetricCode from CDD$ComplexAttribMetric where TableName = '" + tableName + "' and ComplexAttribName = '" + attribName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT t.MetricCode FROM \"CDD$ComplexAttribMetric\" t where t.TableName = '" + tableName + "' AND t.ComplexAttribName = '" + attribName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = getDatabaseManager()->runSelect(strSql);

    if(answer.size() > 0)
    {
        for(int x = 0; x < answer.size(); x++)
        {
            metricCodeAndComplexAttrib.first = answer[x][0];
            metricCodeAndComplexAttrib.second = attribName;
            result.push_back(metricCodeAndComplexAttrib);
        }
    }

    return result;
}

string DataDictionary::getIndexFile(string tableName, string indexName)
{
    string strSql = "";
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT im.IndexFile FROM CDD$ComplexAttribMetric im WHERE im.TableName = '" + tableName + "' AND im.IndexName = '" + indexName + "' AND UPPER(IsDefault)='N'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT im.IndexFile FROM \"CDD$ComplexAttribMetric\" im WHERE im.TableName = '" + tableName + "' AND im.IndexName = '" + indexName + "' AND UPPER(im.IsDefault) = 'N'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = getDatabaseManager()->runSelect(strSql);

    if((answer.size() > 0) && (answer[0].size() > 0)) return answer[0][0];

    return "NULL";
}

string DataDictionary::getIndexFile(string tableName, string attribName, string metricName)
{
    string strSql = "";
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT im.IndexFile FROM CDD$ComplexAttribMetric im, CDD$MetricStruct ms WHERE im.MetricCode = ms.MetricCode AND im.TableName = '" + tableName + "' AND im.ComplexAttribName = '" + attribName + "' AND UPPER(ms.MetricName) = UPPER('" + metricName + "')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT im.IndexFile FROM \"CDD$ComplexAttribMetric\" im, \"CDD$MetricStruct\" ms WHERE im.MetricCode = ms.MetricCode AND im.TableName = '" + tableName + "' AND im.ComplexAttribName = '" + attribName + "' AND UPPER(ms.MetricName) = UPPER('" + metricName + "')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = getDatabaseManager()->runSelect(strSql);

    if((answer.size() > 0) && (answer[0].size() > 0)) return answer[0][0];

    return "NULL";
}

vector<string> DataDictionary::getComplexAttributes(string tableName)
{
    string strSql = "";
    vector<string> result;
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT DISTINCT ComplexAttribName FROM CDD$ComplexAttribute WHERE TableName = '" + tableName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT DISTINCT t.ComplexAttribName FROM \"CDD$ComplexAttribute\" t WHERE t.TableName = '" + tableName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = getDatabaseManager()->runSelect(strSql);

    if(answer.size() > 0)
    {
        for(int x = 0; x < answer.size(); x++)
        {
            result.push_back(answer[x][0]);
        }
    }

    return result;
}

//Checar esquemas de linhas e colunas
vector<string> DataDictionary::getStillimageAttribute(string tableName)
{
    string strSql = "";
    vector<string> result;
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT DISTINCT ComplexAttribName FROM CDD$ComplexAttribute WHERE TableName = '" + tableName + "' AND UPPER(ComplexAttribType) = UPPER('stillimage')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT DISTINCT t.ComplexAttribName FROM \"CDD$ComplexAttribute\" t WHERE t.TableName = '" + tableName + "' AND UPPER(t.ComplexAttribType) = UPPER('stillimage')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = getDatabaseManager()->runSelect(strSql);

    if(answer.size() > 0)
    {
        for(int x = 0; x < answer.size(); x++)
        {
            result.push_back(answer[x][0]);
        }
    }

    return result;
}

vector<string> DataDictionary::getAudioAttribute(string tableName)
{
    string strSql = "";
    vector<string> result;
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT DISTINCT ComplexAttribName FROM CDD$ComplexAttribute WHERE TableName = '" + tableName + "' AND UPPER(ComplexAttribType) = UPPER('audio')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT DISTINCT t.ComplexAttribName FROM \"CDD$ComplexAttribute\" t WHERE t.TableName = '" + tableName + "' AND UPPER(t.ComplexAttribType) = UPPER('audio')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = getDatabaseManager()->runSelect(strSql);

    if(answer.size() > 0)
    {
        for(int x = 0; x < answer.size(); x++)
        {
            result.push_back(answer[x][0]);
        }
    }

    return result;
}

vector<string> DataDictionary::getParticulateAttribute(string tableName)
{
    string strSql = "";
    vector<string> result;
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT DISTINCT ComplexAttribName FROM CDD$ComplexAttribute WHERE TableName = '" + tableName + "' AND UPPER(ComplexAttribType) = UPPER('particulate')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT DISTINCT t.ComplexAttribName FROM \"CDD$ComplexAttribute\" t WHERE t.TableName = '" + tableName + "' AND UPPER(t.ComplexAttribType) = UPPER('particulate')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = getDatabaseManager()->runSelect(strSql);

    if(answer.size() > 0)
    {
        for(int x = 0; x < answer.size(); x++)
        {
            result.push_back(answer[x][0]);
        }
    }

    return result;
}

string DataDictionary::getDistanceFunction(string metricCode)
{
    string strSql = "";
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT DistanceCode FROM CDD$MetricStruct ms WHERE ms.MetricCode = '" + metricCode + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT ms.DistanceCode FROM \"CDD$MetricStruct\" ms WHERE ms.MetricCode = '" + metricCode + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = getDatabaseManager()->runSelect(strSql);

    if((answer.size() > 0) && (answer[0].size() > 0)) return answer[0][0];

    return "NULL";
}

string DataDictionary::getDistanceFunction(string tableName, string attribName, string metricName)
{
    string strSql = "";
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT DistanceCode FROM CDD$ComplexAttribMetric im, CDD$MetricStruct ms WHERE im.MetricCode = ms.MetricCode AND TableName = '" + tableName + "' AND UPPER(ComplexAttribName) = UPPER('" + attribName + "') AND UPPER(ms.MetricName) = UPPER('" + metricName + "')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT ms.DistanceCode FROM \"CDD$ComplexAttribMetric\" im, \"CDD$MetricStruct\" ms WHERE im.MetricCode = ms.MetricCode AND im.TableName = '" + tableName + "' AND UPPER(im.ComplexAttribName) = UPPER('" + attribName + "') AND UPPER(ms.MetricName) = UPPER('" + metricName + "')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = getDatabaseManager()->runSelect(strSql);

    if((answer.size() > 0) && (answer[0].size() > 0)) return answer[0][0];

    return "NULL";
}

string DataDictionary::getParameterType(string parameterName, string extractorCode)
{
    string strSql = "";
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT ParameterType FROM EPD$Parameters WHERE UPPER(parametername) = UPPER('" + parameterName + "') AND extractorcode = " + extractorCode;
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT t.ParameterType FROM \"EPD$Parameters\" t WHERE UPPER(t.ParameterName) = UPPER('" + parameterName + "') AND t.ExtractorCode = " + extractorCode;
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = getDatabaseManager()->runSelect(strSql);

    if((answer.size() > 0) && (answer[0].size() > 0)) return answer[0][0];

    return "NULL";
}

vector< pair<string, string> > DataDictionary::getParameters(string metricCode)
{
    string strSql = "";
    pair<string, string> parameterNameAndParameterType;
    vector< pair<string, string> > result;
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT ParameterName, ParameterType FROM CDD$ParameterStruct WHERE MetricCode = '" + metricCode + "' ORDER BY ParameterOrder";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT t.ParameterName, t.ParameterType FROM \"CDD$ParameterStruct\" t WHERE t.MetricCode = '" + metricCode + "' ORDER BY t.ParameterOrder";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = getDatabaseManager()->runSelect(strSql);

    if(answer.size() > 0)
    {
        for(int x = 0; x < answer.size(); x++)
        {
            parameterNameAndParameterType.first = answer[x][0];
            parameterNameAndParameterType.second = answer[x][1];;
            result.push_back(parameterNameAndParameterType);
        }
    }

    return result;
}

string DataDictionary::getParticleType(string tableName, string particleColName)
{
    string strSql = "";
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT data_type FROM sys.all_tab_columns WHERE owner = user AND UPPER(table_name) = UPPER('" + tableName + "') AND UPPER(column_name) = UPPER('" + particleColName + "')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT t.data_type FROM information_schema.columns t WHERE t.table_name = '" + tableName + "' AND t.column_name = '" + particleColName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = getDatabaseManager()->runSelect(strSql);

    if((answer.size() > 0) && (answer[0].size() > 0)) return answer[0][0];

    return "NULL";
}

vector<string> DataDictionary::getParametersNameForAlterPVTable(string metric, string metricCode)
{
    string strSql = "";
    vector<string> result;
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT ParameterName FROM CDD$ParameterStruct WHERE MetricCode = " + metricCode;
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT t.ParameterName FROM \"CDD$ParameterStruct\" t WHERE t.MetricCode = " + metricCode;
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = (getDatabaseManager()->runSelect(strSql));

    if(answer.size() > 0)
    {
        for(int x = 0; x < answer.size(); x++)
        {
            result.push_back(answer[x][0]);
        }
    }

    return result;
}

int DataDictionary::getNumberOfParticulateParams(string tableName, string attribName, string metricName)
{
    string strSql = "";
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT count(*) amount FROM CDD$MetricStruct ms, CDD$MetricInstance mi WHERE ms.metriccode = mi.metriccode AND mi.tablename = '" + tableName + "' AND mi.complexattribname = '" + attribName + "' AND ms.metricname = '" + metricName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT count(*) AS amount FROM \"CDD$MetricStruct\" ms, \"CDD$MetricInstance\" mi WHERE ms.MetricCode = mi.MetricCode AND mi.TableName = '" + tableName + "' AND mi.ComplexAttribName = '" + attribName + "' AND ms.MetricName = '" + metricName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = getDatabaseManager()->runSelect(strSql);

    if((answer.size() > 0) && (answer[0].size() > 0)) return atoi(answer[0][0].c_str());

    return -1;
}

vector<string> DataDictionary::getParticulateMetricParamNames(string tableName, string metricName, string particulateName)
{
    string strSql = "";
    vector<string> result;
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "select distinct mi.parametername, mi.parameterinstance, ps.parametertype, ps.parameterorder from cdd$metricstruct ms, cdd$metricinstance mi, cdd$parameterstruct ps where ms.metriccode = mi.metriccode and mi.metriccode = ps.metriccode and mi.parametername = ps.parametername and mi.tablename = '" + tableName + "' and ms.metricname = '" + metricName + "' and mi.complexattribname = '" + particulateName + "' order by ps.parameterorder ";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT DISTINCT mi.ParameterName, mi.ParameterInstance, ps.ParameterType, ps.ParameterOrder FROM \"CDD$MetricStruct\" ms, \"CDD$MetricInstance\" mi, \"CDD$ParameterStruct\" ps WHERE ms.MetricCode = mi.MetricCode AND mi.MetricCode = ps.MetricCode AND mi.ParameterName = ps.ParameterName AND mi.TableName = '" + tableName + "' AND ms.MetricName = '" + metricName + "' AND mi.ComplexAttribName = '" + particulateName + "' ORDER BY ps.ParameterOrder ";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = (getDatabaseManager()->runSelect(strSql));

    if(answer.size() > 0)
    {
        for(int x = 0; x < answer.size(); x++)
        {
            result.push_back(answer[x][0]);
        }
    }

    return result;
}

vector<string> DataDictionary::getParticulateMetricParamInstances(string tableName, string metricName, string particulateName)
{
    string strSql = "";
    vector<string> result;
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "select distinct mi.parametername, mi.parameterinstance, ps.parametertype, ps.parameterorder from cdd$metricstruct ms, cdd$metricinstance mi, cdd$parameterstruct ps where ms.metriccode = mi.metriccode and mi.metriccode = ps.metriccode and mi.parametername = ps.parametername and mi.tablename = '" + tableName + "' and ms.metricname = '" + metricName + "' and mi.complexattribname = '" + particulateName + "' order by ps.parameterorder ";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT DISTINCT mi.ParameterName, mi.ParameterInstance, ps.ParameterType, ps.ParameterOrder FROM \"CDD$MetricStruct\" ms, \"CDD$MetricInstance\" mi, \"CDD$ParameterStruct\" ps WHERE ms.MetricCode = mi.MetricCode AND mi.MetricCode = ps.MetricCode AND mi.ParameterName = ps.ParameterName AND mi.TableName = '" + tableName + "' AND ms.MetricName = '" + metricName + "' AND mi.ComplexAttribName = '" + particulateName + "' ORDER BY ps.ParameterOrder ";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = (getDatabaseManager()->runSelect(strSql));

    if(answer.size() > 0)
    {
        for(int x = 0; x < answer.size(); x++)
        {
            result.push_back(answer[x][1]);
        }
    }

    return result;
}

vector<string> DataDictionary::getParticulateMetricParamTypes(string tableName, string metricName, string particulateName)
{
    string strSql = "";
    vector<string> result;
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "select distinct mi.parametername, mi.parameterinstance, ps.parametertype, ps.parameterorder from cdd$metricstruct ms, cdd$metricinstance mi, cdd$parameterstruct ps where ms.metriccode = mi.metriccode and mi.metriccode = ps.metriccode and mi.parametername = ps.parametername and mi.tablename = '" + tableName + "' and ms.metricname = '" + metricName + "' and mi.complexattribname = '" + particulateName + "' order by ps.parameterorder ";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT DISTINCT mi.ParameterName, mi.ParameterInstance, ps.ParameterType, ps.ParameterOrder FROM \"CDD$MetricStruct\" ms, \"CDD$MetricInstance\" mi, \"CDD$ParameterStruct\" ps WHERE ms.MetricCode = mi.MetricCode AND mi.MetricCode = ps.MetricCode AND mi.ParameterName = ps.ParameterName AND mi.TableName = '" + tableName + "' AND ms.MetricName = '" + metricName + "' AND mi.ComplexAttribName = '" + particulateName + "' ORDER BY ps.ParameterOrder ";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = (getDatabaseManager()->runSelect(strSql));

    if(answer.size() > 0)
    {
        for(int x = 0; x < answer.size(); x++)
        {
            result.push_back(answer[x][2]);
        }
    }

    return result;
}

vector< vector<string> > DataDictionary::getParameterNameAndParameterInstance(string tableName, string metricName, string particulateName)
{
    string strSql = "";

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "select distinct mi.parametername, mi.parameterinstance from cdd$metricstruct ms, cdd$metricinstance mi, cdd$parameterstruct ps where ms.metriccode = mi.metriccode and mi.metriccode = ps.metriccode and mi.parametername = ps.parametername and mi.tablename = '" + tableName + "' and ms.metricname = '" + metricName + "' and mi.complexattribname = '" + particulateName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT DISTINCT mi.ParameterName, mi.ParameterInstance FROM \"CDD$MetricStruct\" ms, \"CDD$MetricInstance\" mi, \"CDD$ParameterStruct\" ps WHERE ms.MetricCode = mi.MetricCode AND mi.MetricCode = ps.MetricCode AND mi.ParameterName = ps.ParameterName AND mi.TableName = '" + tableName + "' AND ms.MetricName = '" + metricName + "' AND mi.ComplexAttribName = '" + particulateName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    return getDatabaseManager()->runSelect(strSql);
}

vector< vector<string> > DataDictionary::descTable(string tableName)
{
    string strSql = "";

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "DESC " + tableName;
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT column_name FROM information_schema.columns WHERE table_name = '" + tableName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    return getDatabaseManager()->runSelect(strSql);
}

vector<string> DataDictionary::getParameterName(string tableName, string metricName)
{
    string strSql = "";
    vector<string> result;
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "select distinct mi.parametername from CDD$metricstruct ms, CDD$metricinstance mi where ms.metriccode = mi.metriccode and mi.tablename = '" + tableName + "' and ms.metricname = '" + metricName + "' ";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT DISTINCT mi.ParameterName FROM \"CDD$MetricStruct\" ms, \"CDD$MetricInstance\" mi WHERE ms.MetricCode = mi.MetricCode AND mi.TableName = '" + tableName + "' AND ms.MetricName = '" + metricName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = (getDatabaseManager()->runSelect(strSql));

    if(answer.size() > 0)
    {
        for(int x = 0; x < answer.size(); x++)
        {
            result.push_back(answer[x][0]);
        }
    }

    return result;
}

vector<string> DataDictionary::getParameterInstance(string tableName)
{
    string strSql = "";
    vector<string> result;
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT DISTINCT ParameterInstance FROM CDD$MetricStruct ms, CDD$MetricInstance mi WHERE ms.MetricCode = mi.MetricCode and mi.TableName = '" + tableName + "' ";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT DISTINCT mi.ParameterInstance FROM \"CDD$MetricStruct\" ms, \"CDD$MetricInstance\" mi WHERE ms.MetricCode = mi.MetricCode and mi.TableName = '" + tableName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = (getDatabaseManager()->runSelect(strSql));

    if(answer.size() > 0)
    {
        for(int x = 0; x < answer.size(); x++)
        {
            result.push_back(answer[x][0]);
        }
    }

    return result;
}

vector< vector<string> > DataDictionary::getTableNameCompAttNameMetricCodeIdxFile(string indexName)
{
    string strSql = "";

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "select TableName, ComplexAttribName, MetricCode, IndexFile from CDD$ComplexAttribMetric where IndexName = '" + indexName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT t.TableName, t.ComplexAttribName, t.MetricCode, t.IndexFile FROM \"CDD$ComplexAttribMetric\" t where t.IndexName = '" + indexName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    return getDatabaseManager()->runSelect(strSql);
}

string DataDictionary::getWeight(string metricCode, string parameterName)
{
    string strSql = "";
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT Weight FROM CDD$ParameterStruct ps WHERE metricCode ='" + metricCode + "' AND UPPER(ParameterName) = UPPER('" + parameterName + "')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT ps.weight FROM \"CDD$ParameterStruct\" ps WHERE ps.MetricCode ='" + metricCode + "' AND UPPER(ps.ParameterName) = UPPER('" + parameterName + "')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = getDatabaseManager()->runSelect(strSql);

    if((answer.size() > 0) && (answer[0].size() > 0)) return answer[0][0];

    return "NULL";
}

pair<string, string> DataDictionary::getExtractorNameParameterName(string tableName, string complexAttribName, string metricName)
{
    string strSql = "";
    pair<string, string> aux;
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT DISTINCT ex.ExtractorName, ps.ParameterName FROM CDD$ComplexAttribMetric im, CDD$MetricStruct ms, CDD$ParameterStruct ps, EPD$Extractors ex WHERE im.MetricCode = ms.MetricCode and ms.MetricCode = ps.MetricCode  and ps.ExtractorCode = ex.ExtractorCode and im.TableName = '" + tableName + "' and im.ComplexAttribName = '" + complexAttribName + "' and ms.MetricName = '" + metricName + "' ";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT DISTINCT ex.ExtractorName, ps.ParameterName FROM \"CDD$ComplexAttribMetric\" im, \"CDD$MetricStruct\" ms, \"CDD$ParameterStruct\" ps, \"EPD$Extractors\" ex WHERE im.MetricCode = ms.MetricCode AND ms.MetricCode = ps.MetricCode AND ps.ExtractorCode = ex.ExtractorCode AND im.TableName = '" + tableName + "' AND im.ComplexAttribName = '" + complexAttribName + "' AND ms.MetricName = '" + metricName + "' ";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = getDatabaseManager()->runSelect(strSql);

    if(answer.size() > 0)
    {
        aux.first = answer[0][0];
        aux.second = answer[0][1];
    }

    return aux;
}

vector<string> DataDictionary::getFieldsName(string query)
{
    return getDatabaseManager()->getFieldNames(query);
}

vector<string> DataDictionary::dataDictionaryInDDLDefinition()
{
    vector<string> answer;

    try
    {
        ifstream file("schema.sql", ios_base::in);
        if(!file.good())
        {
            return answer;
        }
        else
        {
            string aux, command;

            while(!file.eof())
            {
                command.clear();
                file >> aux;

                while(aux != ";")
                {
                    if(command.size() > 0) command += " ";
                    command += aux;
                    file >> aux;
                }
                command += aux;
                answer.push_back(command);
            }
            file.close();
        }
        answer.erase(answer.end());
        return answer;
    }
    catch (...)
    {
        return answer;
    }
}

int DataDictionary::getNumberOfPossibleTuples(string query)
{
    return getDatabaseManager()->getNumberOfPossibleTuples(query);
}

string DataDictionary::getEntireBlob(string queryId)
{
    return getDatabaseManager()->getEntireBlob(queryId);
}

vector< vector<string> > DataDictionary::runSubQuery(string subQuery)
{
    return getDatabaseManager()->runSelect(subQuery);
}

string DataDictionary::getValidMetric(string metricName, string tableName, string attributeName)
{
    string strSql = "";
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT Count(*) FROM CDD$ComplexAttribMetric WHERE TableName = '" + tableName + "' AND ComplexAttribName = '" + attributeName + "' AND MetricCode IN (SELECT MetricCode FROM CDD$MetricStruct WHERE MetricName = '" + metricName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT count(*) FROM \"CDD$ComplexAttribMetric\" t1 WHERE t1.TableName = '" + tableName + "' AND t1.ComplexAttribName = '" + attributeName + "' AND t1.MetricCode IN (SELECT t2.MetricCode FROM \"CDD$MetricStruct\" t2 WHERE t2.MetricName = '" + metricName + "'";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = getDatabaseManager()->runSelect(strSql);

    if((answer.size() > 0) && (answer[0].size() > 0)) return answer[0][0];

    return "NULL";
}

string DataDictionary::getClientFolder()
{
    string strSql = "";
    vector< vector<string> > answer;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT Folder FROM TMP$Folder";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT t.Folder FROM \"TMP$Folder\" t";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = getDatabaseManager()->runSelect(strSql);

    if((answer.size() > 0) && (answer[0].size() > 0)) return answer[0][0];

    return "NULL";
}

string DataDictionary::getExtractorParameter(int extractorCode, string parameterName)
{
    string strSql = "";
    vector< vector<string> > answer;
    char aux[8];
    sprintf(aux, "%d", extractorCode);
    string ec = aux;

    if(getDatabaseManager()->getSGBD() == DatabaseManager::MYSQL)
    {
        strSql = "SELECT COUNT(ParameterName) AS AMOUNT FROM EPD$Parameters WHERE ExtractorCode = " + ec + " AND UPPER(ParameterName) = UPPER('" + parameterName + "')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::POSTGRES)
    {
        strSql = "SELECT COUNT(t.ParameterName) AS AMOUNT FROM \"EPD$Parameters\" t WHERE t.ExtractorCode = " + ec + " AND UPPER(t.ParameterName) = UPPER('" + parameterName + "')";
    }
    else if(getDatabaseManager()->getSGBD() == DatabaseManager::ORACLE)
    {
        // @todo
    }

    answer = getDatabaseManager()->runSelect(strSql);

    if((answer.size() > 0) && (answer[0].size() > 0)) return answer[0][0];

    return "NULL";
}

int DataDictionary::getDBMS()
{
    return getDatabaseManager()->getSGBD();
}

void DataDictionary::setDatabaseManager(DatabaseManager *dbmg)
{
    this->dbmg = dbmg;
}

DatabaseManager* DataDictionary::getDatabaseManager()
{
    return dbmg;
}
