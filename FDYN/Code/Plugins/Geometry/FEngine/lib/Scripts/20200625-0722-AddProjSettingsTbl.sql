create table if not exists ProjSettings
(
	name							varchar(30) primary key,
	value							text not null
);

ALTER TABLE FailureOutput ADD parentID integer;
ALTER TABLE FailureOutput ADD type integer;