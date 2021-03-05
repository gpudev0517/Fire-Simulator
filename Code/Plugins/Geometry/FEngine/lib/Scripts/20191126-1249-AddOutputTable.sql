create table if not exists FailureOutput
(
	scenarioID							integer,
	failedItemID						integer,
	failedTime							real,
	foreign key (scenarioID) references ConnectorIDs(ID) on delete cascade,
	foreign key (failedItemID) references ConnectorIDs(ID) on delete cascade 
);

pragma foreign_keys=off;

create table if not exists _brScenarioImpactChanges_new
(
  scenarioConID    integer not null,
  changeItemID     integer not null,
  itemType         integer not null,
  change           integer not null,
  itemParentID     integer not null,
  parentType       integer not null,
  PRIMARY KEY (scenarioConID, changeItemID),
  foreign key(scenarioConID) references Scenarios(connectorID) on delete cascade,
  foreign key(changeItemID) references ConnectorIDs(ID) on delete cascade,
  foreign key(itemParentID) references ConnectorIDs(ID) on delete cascade
);
insert into _brScenarioImpactChanges_new select * from brScenarioImpactChanges;
drop table brScenarioImpactChanges;
alter table _brScenarioImpactChanges_new rename to brScenarioImpactChanges;



pragma foreign_keys=on;