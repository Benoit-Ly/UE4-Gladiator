// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "AIDirector.h"
#include "PlayerCharacter.h"
#include "ArenaFight_GameState.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterKill, FString, AttackerName, FString, VictimName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAISpawned);

UCLASS()
class PROJECT_API AArenaFight_GameState : public AGameState
{
	GENERATED_BODY()

	bool	CheckForLoss();
	bool	CheckForWin();
	bool	CheckForRestart();

public:
	virtual void	GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Unreliable, WithValidation)
	void	Server_OnPlayerDied(APlayerController *playerController);
	UFUNCTION(Category = "Arena Fight|Called by C++", BlueprintImplementableEvent, BlueprintCallable)
	void	OnNoPlayerLeft();

	UFUNCTION(Server, Unreliable, WithValidation)
	void	Server_OnAIDirectorDied(AAIDirector *aiDirector);
	UFUNCTION(Category = "Arena Fight|Called by C++", BlueprintImplementableEvent, BlueprintCallable)
	void	OnNoOpponentLeft();

	UFUNCTION(Server, Unreliable, WithValidation)
	void	Server_OnRestartRequest(APlayerController *playerController);
	UFUNCTION(Category = "Arena Fight|Called by C++", BlueprintImplementableEvent, BlueprintCallable)
	void	OnRestartRequest();
	UFUNCTION(Category = "Arena Fight", BlueprintCallable)
	void	ResetReadyState();

	UFUNCTION(Category = "Arena Fight|Broadcast events", NetMulticast, Unreliable, WithValidation)
	void	ReportCharacterKill(const FString &AttackerName, const FString &VictimName);

	UFUNCTION(Category = "Arena Fight|Broadcast events", NetMulticast, Unreliable, WithValidation)
	void	ReportAISpawned();

	UPROPERTY(BlueprintAssignable)
	FOnCharacterKill OnCharacterKill;

	UPROPERTY(BlueprintAssignable)
	FOnAISpawned	OnAISpawned;

	int		GetNextWave();
	bool	IsGameOver() const;

protected:
	UPROPERTY(Category = "Arena Fight", EditDefaultsOnly, BlueprintReadWrite, Replicated)
	bool	bGameOver = false;

	UPROPERTY(Category = "Arena Fight", EditDefaultsOnly, BlueprintReadWrite)
	int		currWave = 0;

	UPROPERTY(Category = "Arena Fight", BlueprintReadWrite)
	int		numPlayersReady = 0;
};