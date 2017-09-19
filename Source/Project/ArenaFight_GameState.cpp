// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "ArenaFight_GameState.h"
#include "ArenaFight_PlayerState.h"
#include "AIDirector.h"
#include "UnrealNetwork.h"

void	AArenaFight_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArenaFight_GameState, bGameOver);
}

void	AArenaFight_GameState::Server_OnPlayerDied_Implementation(APlayerController *playerController)
{
	if (playerController != NULL)
	{
		AArenaFight_PlayerState	*playerState = Cast<AArenaFight_PlayerState>(playerController->PlayerState);
		
		if (playerState != NULL)
		{
			playerState->bIsAlive = false;

			if (CheckForLoss() == true)
				OnNoPlayerLeft();
		}
	}
}

bool	AArenaFight_GameState::Server_OnPlayerDied_Validate(APlayerController *playerController)
{
	if (playerController != NULL)
		return true;
	return false;
}

bool	AArenaFight_GameState::CheckForLoss()
{
	for (APlayerState *playerState : PlayerArray)
	{
		AArenaFight_PlayerState	*customPlayerState = Cast<AArenaFight_PlayerState>(playerState);

		if (customPlayerState->bIsAlive == true)
		{
			return false;
		}
	}
	return true;
}

void	AArenaFight_GameState::Server_OnAIDirectorDied_Implementation(AAIDirector *aiDirector)
{
	if (aiDirector != NULL)
	{
		if (CheckForWin() == true)
			OnNoOpponentLeft();
	}
}

bool	AArenaFight_GameState::Server_OnAIDirectorDied_Validate(AAIDirector *aiDirector)
{
	if (aiDirector != NULL)
		return true;
	return false;
}

void	AArenaFight_GameState::Server_OnRestartRequest_Implementation(APlayerController *playerController)
{
	if (bGameOver == false)
		return;

	if (playerController != NULL)
	{
		AArenaFight_PlayerState	*playerState = Cast<AArenaFight_PlayerState>(playerController->PlayerState);

		if (playerState != NULL)
		{
			playerState->bIsReady = true;

			if (CheckForRestart() == true)
			{
				OnRestartRequest();
				ResetReadyState();
			}
		}
	}
}

bool	AArenaFight_GameState::Server_OnRestartRequest_Validate(APlayerController* playerController)
{
	if (playerController != nullptr)
		return true;
	return false;
}

bool	AArenaFight_GameState::CheckForRestart()
{
	for (APlayerState *playerState : PlayerArray)
	{
		AArenaFight_PlayerState	*customPlayerState = Cast<AArenaFight_PlayerState>(playerState);

		if (customPlayerState->bIsReady == false)
		{
			return false;
		}
	}
	return true;
}


void	AArenaFight_GameState::ResetReadyState()
{
	for (APlayerState *playerState : PlayerArray)
	{
		AArenaFight_PlayerState	*customPlayerState = Cast<AArenaFight_PlayerState>(playerState);

		customPlayerState->bIsReady = false;
	}
}

bool	AArenaFight_GameState::CheckForWin()
{
	return true;
}

void	AArenaFight_GameState::ReportCharacterKill_Implementation(const FString &AttackerName, const FString &VictimName)
{
	OnCharacterKill.Broadcast(AttackerName, VictimName);
}

bool	AArenaFight_GameState::ReportCharacterKill_Validate(const FString &AttackerName, const FString &VictimName)
{
	return true;
}

void	AArenaFight_GameState::ReportAISpawned_Implementation()
{
	OnAISpawned.Broadcast();
}

bool	AArenaFight_GameState::ReportAISpawned_Validate()
{
	return true;
}

int		AArenaFight_GameState::GetNextWave()
{
	currWave++;
	return currWave;
}

bool	AArenaFight_GameState::IsGameOver() const
{
	return bGameOver;
}