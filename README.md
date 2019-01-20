# SimbA - Extended SQL language processor

**CITATION:** If you want to download and use SimbA for trials or tech report writing, we kindly ask you to include one of the following references.

* Bedo, M. V. N.; Traina, A.; Traina Jr., C. *Seamless Integration of Distance Functions and Feature Vectors for Similarity-Queries Processing*. Journal of Information and Data Management. 2014(5):308-320.
* Barioni, M. C. N.; Razente, H.; Traina, A.; Traina Jr, C. *SIREN: A similarity retrieval engine for complex data*. Proceedings of the 32nd international Conference on Very Large Data Bases 2006:1155-1158.

See open access links [here](https://seer.ufmg.br/index.php/jidm/article/view/689) and [here](http://www.vldb.org/conf/2006/p1155-barioni.pdf).

This repository includes the source code for extending SQL to support similarity searching according to the syntax specification of [SIREN](http://www.vldb.org/conf/2006/p1155-barioni.pdf).
Additionally, the source code also provides a strategy for obtaining a default parse-tree for extended SQL statements to be further optimized according to proper heuristics and statistics.
Metric indexes are provided by [Arboretum Library](https://bitbucket.org/gbdi/arboretum.git).
The strategy works as a *middleware* between DBMSs and users applications themselves.

## Extended SQL commands

In all the following constructs, groups within *[]* are optional terms whereas names starting with *@* are user-defined types previously included on SimbA data dictionary.

**Create Metric Statement**

    <metric-definition> ::= CREATE METRIC <metric-name>
                            [USING {Euclidean | <@your-distance-function> }] FOR
                            {STILLIMAGE '('<list-param-for-extractors>')' |
                            @your-domain-type '('<list-param-for-extractors>')'}
                            
    <metric-name> ::= identifier
    
    <list-param-for-extractors> ::= <extractor-ref> |
                                    <extractor-ref> ',' <list-param-for-extractors>
                                    
    <extractor-ref> ::= <extractor-name> ['(<list-ext-parameters>')']
    
    <extractor-name> ::= @your-extractor-name
    
    <list-ext-parameters> ::= <parameters-ref> |
                              <parameters-ref> ',' <list-ext-parameters>
                              
    <parameters-ref> ::= <parameter-name> <weight>
                         
    <parameter-name> ::= @your-parameter-name
    
    <weight> ::= number
    
**Create Table Statement**

*Metric* are single-table constraints that can be associated with either STILLIMAGE or @your-domain-type data types according to the following syntax.
Clause REFERENCES only admits attributes of the same domain.

    <column-constraint> ::= [<constraint-name>]
                            NULL | PRIMARY KEY | ... |
                            METRIC [ REFERENCES '('<parameter-association-list>')']
                            USING '('<metric-name-list>')
                            
    <constraint-name> ::= identifier
    
    <parameter-association-list> ::= <attribute-name> |
                                     <attribute-name> ',' <association-parameter-list>
    
    <attribute-name> ::= identifier
    
    <parameter-name> ::= identifier
    
    <metric-name-list> ::= <metric-name> |
                           <metric-name> [DEFAULT] ',' <metric-name-list-nd>
                           
    <metric-name-list-nd> ::= <metric-name> |
                              <metric-name> ',' <metric-name-list-nd> 
    
    <metric-name> = identifier
    
    
**Create Index Statement** 

    
    <index-definition> ::= CREATE INDEX <index-name> ON <table-name>
                           { '('<attribute-name>')' [ REFERENCES '('<parameter-association-list>')'] }
                           USING <metric-name> [DEFAULT]
    
    <index-name> ::= identifier
    
    <table-name> ::= identifier
    

**Select Statements**

Similarity selections are described within WHERE clauses, whereas similarity joins are described within FROM clauses.
NEAR - STOP AFTER statements express *k*-nearest neighbor queries, while NEAR - RANGE statements describe range queries.

    <similarity-predicate> ::= <attribute-name>
                               {NEAR | FAR}
                               {<attribute-name> | <attribute-value-ref> |
                                '('<sub-query>')'}
                               [BY <metric-name>]
                               [STOP AFTER <k> [WITH TIE LIST]]
                               [RANGE <r>]
    
    <attribute-value-ref> ::= file-location
    
    <k> ::= natural-number
    
    <r> ::= number
    
    
    <similarity-join> ::= <table-name> [[AS] <alias-table>] 
                                <join-type> 
                          <table-name> [[AS] <alias-table>] ON
                          [<table-name>. | <alias-table>.]<attribute-name> {NEAR | FAR}
                          [<table-name>. | <alias-table>.]<attribute-name> [STOP AFTER <k>] [RANGE <r>]
    
    <join-type> ::= {CLOSEST | NEAREST | RANGE} JOIN


    
**Example - Querying medical images by content on SimbA**

Suppose, for example, you have a scenario that requires the retrieval of mammogram images by content.
Then, one could design a normalized relational schema containing four tables to store the image outcomes of mammogram studies separately.
Each table includes a mammogram image, *i.e.*, a LCC, LMLO, RCC, or RMLO projection.
At the same scenario, suppose the need for two distinct metrics (myMetric1 and myMetric2) so that the first metric is more suitable for the retrieval of similar projections according to Hypothesis-1, while the second metric is solely based on visual patterns usually expected by Hypothesis-2.
Such an example can be implemented through extended SQL statements, as follows:


    CREATE METRIC myMetric1 USING Euclidean FOR STILLIMAGE (HaralickExtractor (Haralick AS h));

    CREATE METRIC myMetric2 USING Canberra FOR 
    STILLIMAGE (HistogramExtractor (Histogram AS h), ZernikeExtractor (|ernike AS z));

    CREATE TABLE lccMammogram (
        id INTEGER, 
        idStudy INTEGER, 
        lcc STILLIMAGE, 
        PRIMARY KEY (id), 
        METRIC (lcc) USING (myMetric1 DEFAULT, myMetric2) 
    );

Therefore, experts can query table lccMammogram by using both perspectives as well as experiment on query parameters only by changing high-level SQL-like statements.
the following commands represent a *3*-nearest neighbor query and a $0.5% range query, respectively.

    SELECT * FROM lccMammogram
    WHERE lcc NEAR ’/Pictures/query-element.jpg’
          BY myMetric1 STOP AFTER 3;
          
    SELECT * FROM lccMammogram
    WHERE lcc NEAR 
          (SELECT rcc from rccMammogram rM WHERE rM.id = 1)
          BY myMetric2 RANGE 0.5;

          
The result set can be easily embedded on expert-customized applications, as the cgi *web* interface in the following figure.
It includes a plain interface for the (A) exploration of the extended SQL expression, (B-C) visualization of both parse-tree and query-plan, and, (D) visualization of query-results.

![][SimbA Example](/FigExampleReadme.png)


          
          














