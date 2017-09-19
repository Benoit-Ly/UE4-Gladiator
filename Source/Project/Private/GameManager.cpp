// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "GameManager.h"
#include "Kismet/KismetSystemLibrary.h"

UGameManager* UGameManager::Instance = nullptr;

UGameManager* UGameManager::GetInstance()
{
	if (!Instance)
	{
		UWorld* World = GEngine->GetWorld();

		if (World != nullptr)
			Instance = Cast<UGameManager>(World->GetGameInstance());
	}

	return Instance;
}

void UGameManager::Init()
{
	for (TSubclassOf<UManager> ManagerClass : this->ManagerList)
		this->CreateManagerFromClass(*ManagerClass);

	for (TSubclassOf<UHelper> HelperClass : this->HelperList)
		this->CreateHelperFromClass(*HelperClass);
}

void UGameManager::CreateManagerFromClass(UClass* ManagerClass)
{
	if (ManagerClass == nullptr)
		return;

	UManager* Manager = NewObject<UManager>(this, ManagerClass);
	this->RegisterManager(Manager);
}

void UGameManager::CreateHelperFromClass(UClass* HelperClass)
{
	if (HelperClass == nullptr)
		return;

	UHelper* Helper = NewObject<UHelper>(this, HelperClass);
	this->RegisterHelper(Helper);
}

void UGameManager::RegisterManager(UManager* newManagerName)
{
	if (newManagerName == nullptr)
		return;

	newManagerName->Init();
	this->ManagerInstances.Add(newManagerName);
}

void UGameManager::RegisterHelper(UHelper* newHelper)
{
	if (newHelper == nullptr)
		return;

	newHelper->Init();
	this->HelperInstances.Add(newHelper);
}

void UGameManager::Shutdown()
{
	Super::Shutdown();

	for (UManager* Manager : this->ManagerInstances)
		Manager->RemoveFromRoot();

	for (UHelper* Helper : this->HelperInstances)
		Helper->RemoveFromRoot();
}

void UGameManager::ExitGame()
{
	UWorld* World = GEngine->GetWorld();

	if (World)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0);
		UKismetSystemLibrary::QuitGame(World, PlayerController, EQuitPreference::Type::Quit);
	}
}