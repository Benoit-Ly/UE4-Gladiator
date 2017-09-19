// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "GameManager.h"
#include "NetworkHelper.h"

UNetworkHelper* UNetworkHelper::Instance = nullptr;

UNetworkHelper* UNetworkHelper::GetInstance()
{
	if (!Instance)
	{
		LOG_ERROR(TEXT("UNetworkHelper class must be added to the GameManager Blueprint"));
		UGameManager::GetInstance()->ExitGame();
	}

	return Instance;
}

void UNetworkHelper::Init()
{
	Instance = this;
	Instance->AddToRoot();
}

FString UNetworkHelper::GetIP()
{
	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get();

	bool bindable = false;
	TSharedRef<FInternetAddr> IPref = SocketSubsystem->GetLocalHostAddr(*GLog, bindable);

	return IPref.Get().ToString(false);
}