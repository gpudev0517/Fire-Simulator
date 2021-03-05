
create table if not exists ZoneBoundaries
(
	connectorID                         integer primary key,
    name                                varchar(100) not null,
    desc                                text not null,
    issues                              text,
    foreign key(connectorID) references ConnectorIDs(ID) on delete cascade
);

DROP TABLE IF EXISTS Compartments;

DROP TABLE IF EXISTS Alarms;
DROP TABLE IF EXISTS Criteria;
