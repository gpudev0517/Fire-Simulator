PRAGMA foreign_keys=off;
create table if not exists _Scenarios_new
(
    connectorID   integer primary key,
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
insert into _Scenarios_new select connectorID, name, issues, desc, run, trmcub, trTrunc, igf, severityFact, nsp, ccdp, cdf, hide, notes from Scenarios;
drop table Scenarios;
alter table _Scenarios_new rename to Scenarios;

PRAGMA foreign_keys=on;


drop table SimInfo;
create table if not exists SimInfo
(
    scenarioID                          integer not null,
    resultEndTime                       integer,
    jsonFieldsStr                       text,
    PRIMARY KEY (scenarioID),
    foreign key(scenarioID) references Scenarios(connectorID) on delete cascade
);