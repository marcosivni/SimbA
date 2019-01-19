# SimbA - Extended SQL language processor

**CITATION:** If you want to download and use SimbA for trials or tech report writing, we kindly ask you to include the following reference.

* Bedo, M. V. N.; Traina, A.; Traina Jr., C. *Seamless Integration of Distance Functions and Feature Vectors for Similarity-Queries Processing*. Journal of Information and Data Management. 2014(5):308-320.

See open access link [here](https://seer.ufmg.br/index.php/jidm/article/view/689).

This repository includes the source code for extending SQL to support similarity searching according to the syntax specification of [SIREN](http://www.vldb.org/conf/2006/p1155-barioni.pdf).
Additionally, the source code also provides a strategy for obtaining a default parse-tree for SQL-extended statements to be further optimized according to proper heuristics and statistics.
Metric indexes are provided by [Arboretum Library](https://bitbucket.org/gbdi/arboretum.git).

## Extended SQL commandos

In all the following constructs, groups within *[]* are optional terms whereas names starting with *@* are user-defined types previously defined on SimbA data dictionary.

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
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    '

