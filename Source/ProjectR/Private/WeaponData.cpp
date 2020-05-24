// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponData.h"
#include "Skill.h"

FWeaponData::FWeaponData()
	: Super()
{
	Skills.SetNum(5);
	Key = 0;
}