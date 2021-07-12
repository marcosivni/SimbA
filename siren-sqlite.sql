CREATE TABLE CDD$Acronym (
  id int(11) NOT NULL,
  caType varchar(50) DEFAULT NULL,
  acronym varchar(3) DEFAULT NULL,
  PRIMARY KEY (id)
);


--- CDD$MetricInstance redundante CDD$ComplexAttribMetric
CREATE TABLE CDD$ComplexAttribMetric (
  TableName varchar(30) NOT NULL,
  ComplexAttribName varchar(30) NOT NULL,
  MetricCode int(11) NOT NULL,
  IsDefault char(1) DEFAULT NULL,
  IndexName varchar(30) DEFAULT NULL,
  IndexFile varchar(30) DEFAULT NULL,
  ComplexAttribDimensionality int(11) DEFAULT NULL,
  PRIMARY KEY (TableName,ComplexAttribName,MetricCode)
);



CREATE TABLE CDD$ComplexAttribute (
  TableName varchar(30) NOT NULL,
  ComplexAttribName varchar(30) NOT NULL,
  ComplexAttribId int(11) NOT NULL,
  PRIMARY KEY (TableName,ComplexAttribName)
);


CREATE TABLE CDD$MetricStruct (
  MetricName varchar(30) UNIQUE NOT NULL,
  DistanceCode int(11) NOT NULL,
  MetricCode int(11) NOT NULL,
  DataTypeId int(11) NOT NULL,
  PRIMARY KEY (MetricCode)
);


CREATE TABLE EPD$DistanceFunctions (
  DistanceName varchar(30) NOT NULL,
  DistanceType varchar(20) NOT NULL,
  DistanceCode int(11) NOT NULL,
  PRIMARY KEY (DistanceCode)
);


CREATE TABLE EPD$DataTypes (
  name varchar(20) NOT NULL DEFAULT '',
  id int(11) DEFAULT '0',
  acronym varchar(3) DEFAULT NULL,
  PRIMARY KEY (id)
);



CREATE TABLE EPD$Sequences (
  name varchar(20) NOT NULL DEFAULT '',
  last_value int(11) DEFAULT '0',
  PRIMARY KEY (name)
);


INSERT INTO CDD$Acronym VALUES (0,'PARTICULATE','PPV');
INSERT INTO EPD$DistanceFunctions VALUES ('Euclidean','metrical',1);
INSERT INTO EPD$DistanceFunctions VALUES ('CityBlock','metrical',2);
INSERT INTO EPD$DistanceFunctions VALUES ('Chebyshev','metrical',3);
INSERT INTO EPD$DistanceFunctions VALUES ('Canberra','metrical',4);
INSERT INTO EPD$DataTypes VALUES ('PARTICULATE', 1);


---- REMOVIDO
CREATE TABLE CDD$MetricInstance (
  TableName varchar(30) NOT NULL,
  ComplexAttribName varchar(30) NOT NULL,
  MetricCode int(11) NOT NULL,
  ComplexAttribDimensionality int(11) DEFAULT NULL,
  PRIMARY KEY (TableName,ComplexAttribName,MetricCode)
);

