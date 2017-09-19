// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "SoundManager.h"
#include "GladiatorWorldSettings.h"
#include "DayNightLevel.h"

void ADayNightLevel::BeginPlay()
{
	Super::BeginPlay();
	
	AGladiatorWorldSettings* WorldSettings = Cast<AGladiatorWorldSettings>(GetWorldSettings());

	USoundManager* SM = USoundManager::GetInstance();

	USoundWave* DefaultBGM = WorldSettings->GetDefaultBGM();
	if (DefaultBGM != nullptr)
	{
		SM->SetDayBGM(this, DefaultBGM, WorldSettings->GetDefaultBGMVolume());
		SM->PlayDayBGM();
	}

	USoundWave* NightBGM = WorldSettings->GetNightBGM();
	if (NightBGM != nullptr)
		SM->SetNightBGM(this, NightBGM, WorldSettings->GetNightBGMVolume());
}

void ADayNightLevel::OnBecomeDay_Implementation()
{
	USoundManager::GetInstance()->PlayDayBGM();
}

void ADayNightLevel::OnBecomeNight_Implementation()
{
	USoundManager::GetInstance()->PlayNightBGM();
}
