// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Manager.h"
#include "SoundManager.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECT_API USoundManager : public UManager
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Managers", meta = (DisplayName = "Get Sound Manager"))
	static USoundManager* GetInstance();
	
	virtual void Init() override;

	UFUNCTION(BlueprintCallable, Category = "Background Music")
	void SetDayBGM(AActor* Owner, USoundWave* NewBGM, float Volume = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Background Music")
	void SetNightBGM(AActor* Owner, USoundWave* NewBGM, float Volume = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Background Music")
	void PlayDayBGM();

	UFUNCTION(BlueprintCallable, Category = "Background Music")
	void PlayNightBGM();

protected:
		

private:
	static USoundManager* Instance;

	UAudioComponent* DayBGM;
	UAudioComponent* NightBGM;
	UAudioComponent* VictoryBGM;
	UAudioComponent* DefeatBGM;

	UAudioComponent* CurrentlyPlayingBGM;

	float DayBGMVolume = 1.0f;
	float NightBGMVolume = 1.0f;

	UAudioComponent* SetBGM(AActor* Owner, USoundWave* NewBGM);
	void DayNightFade(bool IsNight = false) const;
	void FadeTransition(UAudioComponent* Before, UAudioComponent* After) const;
	void StopBGM() const;
};
