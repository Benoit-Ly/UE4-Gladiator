// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "GameManager.h"
#include "FXManager.h"


UFXManager* UFXManager::Instance = nullptr;

UFXManager* UFXManager::GetInstance()
{
	if (!Instance)
	{
		LOG_ERROR(TEXT("UFXManager class must be added to the GameManager Blueprint"));
		UGameManager::GetInstance()->ExitGame();
	}

	return Instance;
}

void UFXManager::Init()
{
	Instance = this;
	Instance->AddToRoot();
}

UParticleSystem* UFXManager::GetDamageEffect()
{
	return this->DamageEffect;
}

UParticleSystem* UFXManager::GetFootstepSandEffect()
{
	return this->FootstepSandEffect;
}

USoundBase* UFXManager::GetDamageSound()
{
	return this->DamageSound;
}

UClass* UFXManager::GetFootstepSandDecal()
{
	return this->FootstepSandDecal;
}

void UFXManager::CallOnItemFar()
{
	OnItemFar.Broadcast();
}

void UFXManager::CallOnItemNear()
{
	OnItemNear.Broadcast();
}

void UFXManager::CallOnLockTarget()
{
	OnLockTarget.Broadcast();
}

void UFXManager::CallOnUnlockTarget()
{
	OnUnlockTarget.Broadcast();
}