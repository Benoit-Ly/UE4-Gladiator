// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "ArenaFight_PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_API AArenaFight_PlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Arena Fight")
	bool	bIsAlive = true;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Arena Fight")
	bool	bIsReady = false;

	virtual void	GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
