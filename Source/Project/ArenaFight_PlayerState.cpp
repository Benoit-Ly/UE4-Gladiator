// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "ArenaFight_PlayerState.h"
#include "UnrealNetwork.h"

void	AArenaFight_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArenaFight_PlayerState, bIsAlive);
	DOREPLIFETIME(AArenaFight_PlayerState, bIsReady);
}
