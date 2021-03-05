create table if not exists Materials
(
	connectorID                         integer primary key,
	name								varchar(100) not null,
	desc								text not null,								
	conductivity						real not null,
	specificHeat						real not null,
	density								real not null,
	emissivity							real not null,
	thickness							real not null, 
	foreign key(connectorID) references ConnectorIDs(ID) on delete cascade
);

CREATE UNIQUE INDEX if not exists tag_MaterialsNames ON Materials (name);

create table if not exists FailureCriteria
(
	connectorID                         integer primary key,
	name								varchar(100) not null,
	desc								text not null,
	temperature							real not null,
	foreign key(connectorID) references ConnectorIDs(ID) on delete cascade
);

CREATE UNIQUE INDEX if not exists tag_FailureCriteriaNames ON FailureCriteria (name);

pragma foreign_keys=off;


alter table FranxCables add materialID integer;
alter table FranxCables add failureCriteriaID integer;
create table _FranxCables_new
(
	connectorID                         integer primary key,
    name                                varchar(100) not null,
    desc                                text not null,
    issues                              text,
	materialID							integer,
	failureCriteriaID					integer,
	foreign key (materialID) references Materials(connectorID) on delete set null,
	foreign key (failureCriteriaID) references FailureCriteria(connectorID) on delete set null,
    foreign key(connectorID) references ConnectorIDs(ID) on delete cascade
);
insert into _FranxCables_new select * from FranxCables;
drop table FranxCables;
alter table _FranxCables_new rename to FranxCables;



alter table FranxComponents add materialID integer;
alter table FranxComponents add failureCriteriaID integer;
create table _FranxComponents_new
(
	connectorID                         integer primary key,
    name                                varchar(100) not null,
    desc                                text not null,
    issues                              text,
	materialID							integer,
	failureCriteriaID					integer,
	foreign key (materialID) references Materials(connectorID) on delete set null,
	foreign key (failureCriteriaID) references FailureCriteria(connectorID) on delete set null,
    foreign key(connectorID) references ConnectorIDs(ID) on delete cascade
);
insert into _FranxComponents_new select * from FranxComponents;
drop table FranxComponents;
alter table _FranxComponents_new rename to FranxComponents;



alter table FranxRaceways add materialID integer;
alter table FranxRaceways add failureCriteriaID integer;
create table _FranxRaceways_new
(
    connectorID                         integer primary key,
    name                                varchar(100) not null,
    desc                                text not null,
    issues                              text,
	materialID							integer,
	failureCriteriaID					integer,
	foreign key (materialID) references Materials(connectorID) on delete set null,
	foreign key (failureCriteriaID) references FailureCriteria(connectorID) on delete set null,
    foreign key(connectorID) references ConnectorIDs(ID) on delete cascade
);
insert into _FranxRaceways_new select * from FranxRaceways;
drop table FranxRaceways;
alter table _FranxRaceways_new rename to FranxRaceways;

pragma foreign_keys=on;