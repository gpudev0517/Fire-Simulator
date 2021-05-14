create table if not exists SimulationItems
(
    connectorID                         integer primary key,
	  name								                varchar(100) not null,
	  desc								                text not null,
    itemType                            integer not null,
    jsonFieldsStr                       text,
    foreign key(connectorID) references ConnectorIDs(ID) on delete cascade
);

create table if not exists IndirectReferences
(
	mainItemID							integer not null,
	subItemID							integer not null, 
	primary key (mainItemID, subItemID),
	foreign key (mainItemID) references ConnectorIDs(ID) on delete cascade,
	foreign key (subItemID) references ConnectorIDs(ID) on delete cascade
);