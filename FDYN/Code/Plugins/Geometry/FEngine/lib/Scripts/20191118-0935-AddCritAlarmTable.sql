create table if not exists Criteria
(
	connectorID                         integer primary key,
	name								varchar(100) not null,
	desc								text not null,								
	cfastTime							text,
	cfastTemperature					text,
	cfastHeatFlux						text,
	foreign key(connectorID) references ConnectorIDs(ID) on delete cascade
);

CREATE UNIQUE INDEX if not exists tag_Criteria ON Criteria (name);

create table if not exists Alarms
(
	connectorID                         integer primary key,
	name								varchar(100) not null,
	desc								text not null,								
	cfastSmoke							text,
	cfastHeat							text,
	cfastSprinkler						text,
	foreign key(connectorID) references ConnectorIDs(ID) on delete cascade
);

CREATE UNIQUE INDEX if not exists tag_Alarms ON Alarms (name);