// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "GameManager.h"
#include "SoundManager.h"

USoundManager* USoundManager::Instance = nullptr;

USoundManager* USoundManager::GetInstance()
{
	if (!Instance)
	{
		LOG_ERROR(TEXT("USoundManager class must be added to the GameManager Blueprint"));
		UGameManager::GetInstance()->ExitGame();
	}

	return Instance;
}

void USoundManager::Init()
{
	Instance = this;
	Instance->AddToRoot();
}

void USoundManager::SetDayBGM(AActor* Owner, USoundWave* NewBGM, float Volume)
{
	if (NewBGM == nullptr)
		return;

	this->DayBGM = this->SetBGM(Owner, NewBGM);
	this->DayBGM->AdjustVolume(0.f, 0.f);
	this->DayBGMVolume = Volume;
}

void USoundManager::SetNightBGM(AActor* Owner, USoundWave* NewBGM, float Volume)
{
	if (NewBGM == nullptr)
		return;

	this->NightBGM = this->SetBGM(Owner, NewBGM);
	this->NightBGM->AdjustVolume(0.f, 0.f);
	this->NightBGMVolume = Volume;
}

void USoundManager::PlayDayBGM()
{
	if (!this->DayBGM)
		return;

	if (!this->DayBGM->IsPlaying())
		this->DayBGM->FadeIn(2.0f, this->DayBGMVolume);
	else
		this->DayNightFade(false);
}

void USoundManager::PlayNightBGM()
{
	if (!this->NightBGM)
		return;

	if (!this->NightBGM->IsPlaying())
	{
		this->NightBGM->FadeIn(2.0f, this->NightBGMVolume);
		this->DayBGM->AdjustVolume(2.0f, 0.f);
	}
	else
		this->DayNightFade(true);
}

UAudioComponent* USoundManager::SetBGM(AActor* Owner, USoundWave* NewBGM)
{
	if (NewBGM == nullptr)
		return nullptr;

	UAudioComponent* NewAudioComp = NewObject<UAudioComponent>(Owner);
	NewAudioComp->RegisterComponent();
	NewAudioComp->SetSound(NewBGM);

	return NewAudioComp;
}

void USoundManager::DayNightFade(bool IsNight) const
{
	if (this->DayBGM == nullptr || this->NightBGM == nullptr)
		return;

	if (IsNight)
	{
		this->DayBGM->AdjustVolume(2.0f, 0.f);
		this->NightBGM->AdjustVolume(2.0f, this->NightBGMVolume);
	}
	else
	{
		this->NightBGM->AdjustVolume(2.0f, 0.f);
		this->DayBGM->AdjustVolume(2.0f, this->DayBGMVolume);
	}
}

void USoundManager::StopBGM() const
{
	if (this->CurrentlyPlayingBGM != nullptr && this->CurrentlyPlayingBGM->IsPlaying())
		this->CurrentlyPlayingBGM->Stop();
}