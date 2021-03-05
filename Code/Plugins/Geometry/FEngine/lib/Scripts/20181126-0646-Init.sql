create table if not exists MigrationScripts
(
    name                                varchar(100) not null,
    executionDate                       dateTime not null
);

create table if not exists ConnectorIDs
(
    ID                                  integer primary key AUTOINCREMENT
);

create table if not exists Zones
(
    connectorID                         integer primary key,
    name                                varchar(100) not null,
    desc                                text not null, 
    issues                              text,
    foreign key(connectorID) references ConnectorIDs(ID) on delete cascade
);

create table if not exists Scenarios
(
    connectorID   integer not null,
    name          varchar(100) not null,
    issues        text not null,
    desc          text not null,
    run           bool not null,
    trmcub        real,
    trTrunc       real,
    igf           real not null,
    severityFact  real not null,   
    nsp           real not null,
    ccdp          real not null,
    cdf           real not null,
    hide          bool not null,
    notes         text,
    foreign key(connectorID) references ConnectorIDs(ID) on delete cascade
);

create table if not exists FranxRaceways
(
    connectorID                         integer primary key,
    name                                varchar(100) not null,
    desc                                text not null,
    issues                              text,
    foreign key(connectorID) references ConnectorIDs(ID) on delete cascade
);

create table if not exists FranxCables
(
    connectorID                         integer primary key,
    name                                varchar(100) not null,
    desc                                text not null,
    issues                              text,
    foreign key(connectorID) references ConnectorIDs(ID) on delete cascade
);

create table if not exists FranxComponents
(
    connectorID                         integer primary key,
    name                                varchar(100) not null,
    desc                                text not null,
    issues                              text,
    foreign key(connectorID) references ConnectorIDs(ID) on delete cascade
);

create table if not exists BasicEvents
(
    connectorID                         integer primary key,
    name                                varchar(100) not null,
    desc                                text not null,
    issues                              text,
    foreign key(connectorID) references ConnectorIDs(ID) on delete cascade
);

create table if not exists brConnectors 
(
  fromConID    integer not null,
  toConID      integer not null,
  PRIMARY KEY (fromConID, toConID),
  foreign key(fromConID) references ConnectorIDs(ID) on delete cascade,
  foreign key(toConID) references ConnectorIDs(ID) on delete cascade  
);

create table if not exists brScenarioImpactChanges
(
  scenarioConID    integer not null,
  changeItemID     integer not null,
  itemType         integer not null,
  change           integer not null,
  itemParentID     integer not null,
  parentType       integer not null,
  PRIMARY KEY (scenarioConID, changeItemID),
  foreign key(scenarioConID) references Scenarios(scenarioConID) on delete cascade,
  foreign key(changeItemID) references ConnectorIDs(changeItemID) on delete cascade,
  foreign key(itemParentID) references ConnectorIDs(itemParentID) on delete cascade
);

CREATE UNIQUE INDEX tag_RacewayNames ON FranxRaceways (name);
CREATE UNIQUE INDEX tag_RacewayNamestag_ZoneNames ON Zones (name);
CREATE UNIQUE INDEX tag_RacewayNamestag_ScenarioNames ON Scenarios (name);
CREATE UNIQUE INDEX tag_RacewayNamestag_CableNames ON FranxCables (name);
CREATE UNIQUE INDEX tag_RacewayNamestag_ComponentNames ON FranxComponents (name);
CREATE UNIQUE INDEX tag_RacewayNamestag_BasicEventNames ON BasicEvents (name);
