// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/LevelScriptActor.h"
#include "DayNightLevel.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_API ADayNightLevel : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Day/Night Cycle")
		void OnBecomeDay();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Day/Night Cycle")
		void OnBecomeNight();

protected:
};
