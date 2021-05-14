PRAGMA foreign_keys=off;
create table if not exists _FranxCables_new
(
  connectorID                         integer primary key,
  name                                varchar(100) not null,
  desc                                text not null,
  issues                              text,
	cableSpecsID							          integer,
	foreign key (cableSpecsID) references CableSpecs(connectorID) on delete set null,
	foreign key(connectorID) references ConnectorIDs(ID) on delete cascade
);
insert into _FranxCables_new(connectorID, name, desc, issues) select connectorID, name, desc, issues from FranxCables;
drop table FranxCables;
alter table _FranxCables_new rename to FranxCables;

PRAGMA foreign_keys=on;