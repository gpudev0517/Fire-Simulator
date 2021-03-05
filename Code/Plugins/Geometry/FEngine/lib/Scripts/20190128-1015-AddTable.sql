create table if not exists PlantModelItems
(
    connectorID                         integer primary key,
    name                                varchar(100) not null,
    desc                                text not null,
    issues                              text not null,
    pItemType                           integer not null,
    jsonFieldsStr                          text,
    foreign key(connectorID) references ConnectorIDs(ID) on delete cascade
);

CREATE UNIQUE INDEX if not exists tag_PlantModelItemsNames ON PlantModelItems (name);

create table if not exists PhysicalOnlyItems
(
    connectorID                         integer primary key,
    name                                varchar(100) not null,
    desc                                text not null,
    issues                              text not null,
    jsonFieldsStr                          text,
    foreign key(connectorID) references ConnectorIDs(ID) on delete cascade
);

CREATE UNIQUE INDEX if not exists tag_PhysicalOnlyItemsNames ON PhysicalOnlyItems (name);