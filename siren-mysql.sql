CREATE DATABASE SIREN;

DROP TABLE IF EXISTS CDD$Acronym
;
CREATE TABLE CDD$Acronym (
  id int(11) NOT NULL,
  caType varchar(50) DEFAULT NULL,
  acronym varchar(3) DEFAULT NULL,
  PRIMARY KEY (id),
);

LOCK TABLES CDD$Acronym WRITE
;

INSERT INTO CDD$Acronym VALUES (0,'STILLIMAGE','IPV')
;

UNLOCK TABLES
;

DROP TABLE IF EXISTS CDD$ComplexAttribMetric
;

CREATE TABLE CDD$ComplexAttribMetric (
  TableName varchar(30) NOT NULL,
  ComplexAttribName varchar(30) NOT NULL,
  MetricCode int(11) NOT NULL,
  IsDefault char(1) DEFAULT NULL,
  IndexName varchar(30) DEFAULT NULL,
  IndexFile varchar(30) DEFAULT NULL,
  PRIMARY KEY (TableName,ComplexAttribName,MetricCode),
  UNIQUE KEY IndexName (IndexName),
  KEY MetricCode (MetricCode)
)
;

LOCK TABLES CDD$ComplexAttribMetric WRITE
;

UNLOCK TABLES
;

DROP TABLE IF EXISTS CDD$ComplexAttribute
;

CREATE TABLE CDD$ComplexAttribute (
  TableName varchar(30) NOT NULL,
  ComplexAttribName varchar(30) NOT NULL,
  ComplexAttribType varchar(15) NOT NULL,
  PRIMARY KEY (TableName,ComplexAttribName)
)
;


LOCK TABLES CDD$ComplexAttribute WRITE
;

UNLOCK TABLES
;

DROP TABLE IF EXISTS CDD$MetricInstance
;

CREATE TABLE CDD$MetricInstance (
  TableName varchar(30) NOT NULL,
  ComplexAttribName varchar(30) NOT NULL,
  MetricCode int(11) NOT NULL,
  ParameterName varchar(30) NOT NULL,
  ExtractorCode int(11) NOT NULL,
  ParameterInstance varchar(30) DEFAULT NULL,
  PRIMARY KEY (TableName,ComplexAttribName,MetricCode,ParameterName,ExtractorCode),
  KEY MetricCode (MetricCode,ParameterName,ExtractorCode)
)
;

LOCK TABLES CDD$MetricInstance WRITE
;

UNLOCK TABLES
;

DROP TABLE IF EXISTS CDD$MetricStruct
;
CREATE TABLE CDD$MetricStruct (
  MetricName varchar(30) NOT NULL,
  MetricType varchar(20) NOT NULL,
  DistanceCode int(11) NOT NULL,
  MetricCode int(11) NOT NULL,
  PRIMARY KEY (MetricCode),
  KEY DistanceCode (DistanceCode)
)
;

LOCK TABLES CDD$MetricStruct WRITE
;
UNLOCK TABLES
;

DROP TABLE IF EXISTS CDD$ParameterStruct
;

CREATE TABLE CDD$ParameterStruct (
  MetricCode int(11) NOT NULL,
  ExtractorCode int(11) NOT NULL,
  ParameterName varchar(30) NOT NULL,
  ParameterAlias varchar(30) DEFAULT NULL,
  ParameterType varchar(20) DEFAULT NULL,
  ParameterOrder decimal(10,0) DEFAULT NULL,
  Weight decimal(10,4) DEFAULT NULL,
  PRIMARY KEY (MetricCode,ExtractorCode,ParameterName)
)
;

LOCK TABLES CDD$ParameterStruct WRITE
;
UNLOCK TABLES
;

DROP TABLE IF EXISTS EPD$DistanceFunctions
;

CREATE TABLE EPD$DistanceFunctions (
  DistanceName varchar(30) NOT NULL,
  DistanceType varchar(20) NOT NULL,
  DistanceCode int(11) NOT NULL,
  PRIMARY KEY (DistanceCode)
)
;

LOCK TABLES EPD$DistanceFunctions WRITE
;

INSERT INTO EPD$DistanceFunctions VALUES ('Euclidean','metrical',1),('CityBlock','metrical',2),('Chebyshev','metrical',3),('MHD','metrical',4),('Jeffrey','nonmetrical',5),('Canberra','nonmetrical',6)
;

UNLOCK TABLES
;


DROP TABLE IF EXISTS EPD$Extractors
;

CREATE TABLE EPD$Extractors (
  ExtractorName varchar(30) NOT NULL,
  ExtractorType varchar(20) NOT NULL,
  ExtractorCode int(11) NOT NULL,
  PRIMARY KEY (ExtractorCode)
)
;


LOCK TABLES EPD$Extractors WRITE
;

INSERT INTO EPD$Extractors VALUES ('histogramext','stillimage',1),('metrichistogramext','stillimage',2),('colorhistogramext','stillimage',3),('zernikeext','stillimage',4),('haralickext','stillimage',5),('waveletshaarext','stillimage',6),('waveletsdaubehiesext','stillimage',7),('mpegext','stillimage',8),('soundtextureext','audio',9),('pcaext','audio',10),('gapext','series',11)
;

UNLOCK TABLES
;


DROP TABLE IF EXISTS EPD$Parameters
;

CREATE TABLE EPD$Parameters (
  ExtractorCode int(11) NOT NULL,
  ParameterName varchar(30) NOT NULL,
  ParameterType varchar(30) DEFAULT NULL,
  IsIndexable char(1) NOT NULL,
  PRIMARY KEY (ExtractorCode,ParameterName)
)
;


LOCK TABLES EPD$Parameters WRITE
;

INSERT INTO EPD$Parameters VALUES (1,'histogram','double','Y'),(2,'metrichistogram','double','Y'),(11,'gap','double','Y'),(4,'zernike','double','Y'),(5,'haralick','double','Y')
;

UNLOCK TABLES
;


DROP TABLE IF EXISTS EPD$Sequences
;

CREATE TABLE EPD$Sequences (
  name varchar(20) NOT NULL DEFAULT '',
  last_value int(11) DEFAULT '0',
  PRIMARY KEY (name)
)
;

LOCK TABLES EPD$Sequences WRITE
;

UNLOCK TABLES
;
