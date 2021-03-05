create table if not exists FireMaterials
(
	connectorID                         integer primary key,
	name								varchar(100) not null,
	desc								text not null,								
	cfastPropertiesJson						text,
	foreign key(connectorID) references ConnectorIDs(ID) on delete cascade
);

CREATE UNIQUE INDEX if not exists tag_FireMaterialsNames ON FireMaterials (name);

pragma foreign_keys = off;

alter table PlantModelItems add fireMaterialID integer;

create table if not exists _PlantModelItems_new
(
    connectorID                         integer primary key,
    name                                varchar(100) not null,
    desc                                text not null,
    issues                              text not null,
    pItemType                           integer not null,
    jsonFieldsStr                       text,
	fireMaterialID						integer,
	foreign key (fireMaterialID) references FireMaterials(connectorID) on delete set null,
    foreign key(connectorID) references ConnectorIDs(ID) on delete cascade
);
insert into _PlantModelItems_new select * from PlantModelItems;
drop table PlantModelItems;
alter table _PlantModelItems_new rename to PlantModelItems;

pragma foreign_keys = on;