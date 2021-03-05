create table if not exists Compartments
(
    connectorID                         integer primary key,
    name                                varchar(100) not null,
    desc                                text not null,
    issues                              text,
	wallMaterialID						integer,
	floorMaterialID						integer,
	ceilingMaterialID					integer,
	foreign key (wallMaterialID) references Materials(connectorID) on delete set null,
	foreign key (floorMaterialID) references Materials(connectorID) on delete set null,
	foreign key (ceilingMaterialID) references Materials(connectorID) on delete set null,
    foreign key(connectorID) references ConnectorIDs(ID) on delete cascade
);