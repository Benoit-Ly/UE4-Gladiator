// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "GladiatorWorldSettings.h"

USoundWave* AGladiatorWorldSettings::GetDefaultBGM()
{
	return this->DefaultBGM;
}

USoundWave* AGladiatorWorldSettings::GetNightBGM()
{
	return this->NightBGM;
}

float AGladiatorWorldSettings::GetDefaultBGMVolume()
{
	return this->DefaultBGMVolume;
}

float AGladiatorWorldSettings::GetNightBGMVolume()
{
	return this->NightBGMVolume;
}