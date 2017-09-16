create table "CDD$Acronym" (
	id integer primary key,
	caType character varying (50) unique default null,
	acronym character varying (3) unique default null
);

create table "CDD$ComplexAttribute" (
	TableName character varying (30),
	ComplexAttribName character varying (30),
	ComplexAttribType character varying (15) not null,
	primary key (TableName, ComplexAttribName)
);

create table "CDD$ComplexAttribMetric" (
	TableName character varying (30),
	ComplexAttribName character varying (30),
	MetricCode integer unique,
	IsDefault character (1) default null,
	IndexName character varying (30) unique default null,
	IndexFile character varying (30) default null,
	primary key (TableName, ComplexAttribName, MetricCode)
);

create table "CDD$MetricInstance" (
	TableName character varying (30) not null,
	ComplexAttribName character varying (30) not null,
	MetricCode integer not null,
	ParameterName character varying (30) not null,
	ExtractorCode integer not null,
	ParameterInstance character varying (30) default null,
	primary key (TableName, ComplexAttribName, MetricCode, ParameterName, ExtractorCode)
);

create table "CDD$MetricStruct" (
	MetricName character varying (30) not null,
	MetricType character varying (20) not null,
	DistanceCode integer not null,
	MetricCode integer primary key
);

create table "CDD$ParameterStruct" (
	MetricCode integer,
	ExtractorCode integer,
	ParameterName character varying (30),
	ParameterAlias character varying (30) default null,
	ParameterType character varying (20) default null,
	ParameterOrder float default null,
	Weight float default null,
	primary key (MetricCode, ExtractorCode, ParameterName)
);

create table "EPD$DistanceFunctions" (
	DistanceName character varying (30) not null,
	DistanceType character varying (20) not null,
	DistanceCode integer primary key
);

create table "EPD$Extractors" (
	ExtractorName character varying (30) not null,
	ExtractorType character varying (20) not null,
	ExtractorCode integer primary key
);

create table "EPD$Parameters" (
	ExtractorCode integer,
	ParameterName character varying (30),
	ParameterType character varying (30) not null,
	IsIndexable character (1) not null,
	primary key (ExtractorCode, ParameterName)
);

create table "EPD$Sequences" (
	name character varying (20) primary key,
	last_value integer
);

insert into "CDD$Acronym" values (0, 'STILLIMAGE', 'IPV');

insert into "EPD$DistanceFunctions" values ('Euclidean','metrical',1),('CityBlock','metrical',2),('Chebyshev','metrical',3),('MHD','metrical',4),('Jeffrey','nonmetrical',5),('Canberra','nonmetrical',6);

insert into "EPD$Extractors" values ('histogramext','stillimage',1),('metrichistogramext','stillimage',2),('colorhistogramext','stillimage',3),('zernikeext','stillimage',4),('haralickext','stillimage',5),('waveletshaarext','stillimage',6),('waveletsdaubehiesext','stillimage',7),('mpegext','stillimage',8),('soundtextureext','audio',9),('pcaext','audio',10),('gapext','series',11);

insert into "EPD$Parameters" values (1,'histogram','double','Y'),(2,'metrichistogram','double','Y'),(11,'gap','double','Y'),(4,'zernike','double','Y'),(5,'haralick','double','Y');

