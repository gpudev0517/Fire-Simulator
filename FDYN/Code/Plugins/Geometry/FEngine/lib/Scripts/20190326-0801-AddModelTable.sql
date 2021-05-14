create table if not exists Model3DInfo
(
    connectorID                         integer not null,
    zoneID                              integer not null,
    jsonFieldsStr                          text,
    PRIMARY KEY (connectorID, zoneID),
    foreign key(connectorID) references ConnectorIDs(ID) on delete cascade
    foreign key(zoneID) references ConnectorIDs(ID) on delete cascade
);

create table if not exists SimInfo
(
    connectorID   integer not null,
    name          varchar(100) not null,
    desc          text not null,
    forObjType    integer not null,
    jsonFieldsStr text,
    PRIMARY KEY (connectorID),
    foreign key(connectorID) references ConnectorIDs(ID) on delete cascade
);

CREATE UNIQUE INDEX if not exists tag_SimInfoNames ON SimInfo (name);