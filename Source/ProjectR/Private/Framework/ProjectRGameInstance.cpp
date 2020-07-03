// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/ProjectRGameInstance.h"

UDataTable* UProjectRGameInstance::GetDataTable(FName Name) const
{
	auto Table = DataTables.Find(Name);
	return Table ? *Table : nullptr;
}
