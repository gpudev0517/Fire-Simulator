ALTER TABLE ZoneBoundaries
  ADD jsonFieldsStr text;

create table if not exists CableSpecs
(
  connectorID         integer primary key,
  name								varchar(100) not null,
  category            integer not null,							
  manufacturer        text,
  model								text,
  insulType           integer not null,
  jackType            integer not null,
  od                  real not null,
  kgm                 real not null,
  jackThick           real not null,  
  guage               integer,
  conductors          integer,
  grade               text,
  partNum             text,
  itemNum             text,
  
  
  foreign key(connectorID) references ConnectorIDs(ID) on delete cascade
);

CREATE INDEX tag_CableSpecsType ON CableSpecs (insulType);
CREATE INDEX tag_CableSpecsGuage ON CableSpecs (guage);
CREATE INDEX tag_CableSpecsConductors ON CableSpecs (conductors);
CREATE INDEX tag_CableSpecsSearch ON CableSpecs (insulType, guage, conductors);
CREATE INDEX tag_CableSpecsPartNum ON CableSpecs (partNum);
CREATE INDEX tag_CableSpecsItemNum ON CableSpecs (itemNum);
