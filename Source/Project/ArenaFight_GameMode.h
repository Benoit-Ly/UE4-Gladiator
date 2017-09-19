// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "ArenaFight_GameMode.generated.h"

class	AAIDirector;

namespace	MatchState
{
	const FName	ReadyToStart = FName("ReadyToStart");
};

/**
 * 
 */
UCLASS()
class PROJECT_API AArenaFight_GameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AArenaFight_GameMode();

	virtual void	BeginPlay() override;

#pragma region Seamless Travel Handling
	virtual void	PostSeamlessTravel() override;

	virtual void	HandleSeamlessTravelPlayer(AController *&travelledPlayer) override;
#pragma endregion

	virtual void	PostLogin(APlayerController *newPlayer) override;

	virtual void	HandleMatchIsWaitingToStart() override;

	virtual void	HandleMatchHasStarted() override;

	virtual void	OnMatchStateSet() override;

protected:
	// Is there enough players in listPlayers ?
	UFUNCTION(Category = "Arena Fight", BlueprintCallable)
	bool	HasEnoughPlayers() const;

	// Used to bypass issue where BeginPlay() is not called first
	UFUNCTION(Category = "Arena Fight", BlueprintCallable)
	void	CustomBeginPlay();

	// Parse OptionsString given when launching Server
	UFUNCTION(Category = "Arena Fight", BlueprintCallable)
	void	ParseLaunchOptions();

	// Launches game, and call initialize Players and AIs. Sets MatchState from WaitingToStart to ReadyToStart.
	UFUNCTION(Category = "Arena Fight", BlueprintCallable)
	void	SetupAndStartGame();

	// Creates a APlayerCharacter for every player in listPlayer
	UFUNCTION(Category = "Arena Fight", BlueprintCallable)
	void	SetupPlayers();

	// StartMatch() equivalent, but state doesn't need to be WaitingToStart
	UFUNCTION(Category = "Arena Fight", BlueprintCallable)
	void	CustomStartMatch();

	// Tell AIDirector to start next wave
	UFUNCTION(Category = "Arena Fight", BlueprintCallable)
	void	StartNextWave();

	// Tells every Controller (Player or AI) to pause themselves
	UFUNCTION(Category = "Arena Fight", BlueprintCallable)
	void	SetPauseAllControllers(const bool bPause);

	UFUNCTION(Category = "Arena Fight", BlueprintCallable)
	void	ClientsUseMatinee(const bool bEnable);
	
protected:
	UPROPERTY(Category = "Arena Fight", BlueprintReadWrite, VisibleAnywhere)
	int			PlayersWanted = 1;

	UPROPERTY(Category = "Arena Fight", BlueprintReadWrite, VisibleAnywhere)
	bool		bIsInit = false;

	UPROPERTY(Category = "Arena Fight", BlueprintReadWrite, VisibleAnywhere)
	AAIDirector	*aiDirector = nullptr;

	UPROPERTY(Category = "Arena Fight", BlueprintReadWrite, VisibleAnywhere)
	TArray<APlayerController *>	listPlayers;
};
