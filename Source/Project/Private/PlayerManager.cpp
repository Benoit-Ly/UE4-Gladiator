// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "GameManager.h"
#include "PlayerManager.h"

UPlayerManager* UPlayerManager::Instance = nullptr;

UPlayerManager* UPlayerManager::GetInstance()
{
	if (!Instance)
	{
		LOG_ERROR(TEXT("UPlayerManager class must be added to the GameManager Blueprint"));
		UGameManager::GetInstance()->ExitGame();
	}

	return Instance;
}

void UPlayerManager::Init()
{
	Instance = this;
	Instance->AddToRoot();
}

void UPlayerManager::SetPlayerName(FString Name)
{
	this->PlayerName = Name;
}

FString UPlayerManager::GetPlayerName() const
{
	return this->PlayerName;
}