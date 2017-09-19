// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "GameManager.h"
#include "GUIManager.h"

UGUIManager* UGUIManager::Instance = nullptr;

UGUIManager* UGUIManager::GetInstance()
{
	if (!Instance)
	{
		LOG_ERROR(TEXT("UGUIManager class must be added to the GameManager Blueprint"));
		UGameManager::GetInstance()->ExitGame();
	}

	return Instance;
}

void UGUIManager::Init()
{
	Instance = this;
	Instance->AddToRoot();
}

void UGUIManager::ReportDisappear(UUserWidget* widget)
{
	this->OnMessageDisappear.Broadcast(widget);
}