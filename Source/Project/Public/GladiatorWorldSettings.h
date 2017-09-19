// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/WorldSettings.h"
#include "GladiatorWorldSettings.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_API AGladiatorWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
	
public:
	USoundWave* GetDefaultBGM();
	USoundWave* GetNightBGM();

	float GetDefaultBGMVolume();
	float GetNightBGMVolume();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
		USoundWave* DefaultBGM = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
		float DefaultBGMVolume = 1.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
		USoundWave* NightBGM = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
		float NightBGMVolume = 1.0f;
};
