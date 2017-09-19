// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Manager.h"
#include "PlayerManager.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_API UPlayerManager : public UManager
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Managers", meta = (DisplayName = "Get Player Manager"))
	static UPlayerManager* GetInstance();

	virtual void Init() override;

	UFUNCTION(BlueprintCallable, Category = "Player Manager")
	void SetPlayerName(FString Name);

	FString GetPlayerName() const;

private:
	static UPlayerManager* Instance;
 
	FString PlayerName = "Player";
	
};
