pragma foreign_keys = off;

alter table Scenarios add simInfoID integer;
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
	simInfoID	  integer,
	foreign key(simInfoID) references SimInfo(connectorID) on delete set null,
    foreign key(connectorID) references ConnectorIDs(ID) on delete cascade
);
insert into _Scenarios_new select * from Scenarios;
drop table Scenarios;
alter table _Scenarios_new rename to Scenarios;

pragma foreign_keys = on;