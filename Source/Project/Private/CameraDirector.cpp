// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "CameraDirector.h"


// Sets default values
ACameraDirector::ACameraDirector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ACameraDirector::BeginPlay()
{
	Super::BeginPlay();
	CamManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	PlayerCamera = CamManager->GetViewTarget();
	SwitchToMatinee();
}

void ACameraDirector::SwitchToMatinee()
{
	if (!MatineeCamera)
		LOG_TEMP(TEXT("NoMatinee"));
	if (!CamManager)
		LOG_TEMP(TEXT("NoManager"));

	if (!CamManager || !MatineeCamera)
		return;

	LOG_TEMP(TEXT("SwitchToMatinee"));
	FViewTargetTransitionParams TransParams;
	TransParams.BlendTime = 0.5f;
	TransParams.BlendExp = 2.0f;
	TransParams.BlendFunction = EViewTargetBlendFunction::VTBlend_EaseInOut;

	CamManager->SetViewTarget (MatineeCamera, TransParams);
}

void ACameraDirector::SwitchToPlayerCamera()
{
	if (!PlayerCamera)
		LOG_TEMP(TEXT("NoCamera"));
	if (!CamManager)
		LOG_TEMP(TEXT("NoManager"));
	if (!CamManager || !PlayerCamera)
		return;

	LOG_TEMP(TEXT("SwitchToPlayer"));

	FViewTargetTransitionParams TransParams;
	TransParams.BlendTime = 0.5f;
	TransParams.BlendExp = 2.0f;
	TransParams.BlendFunction = EViewTargetBlendFunction::VTBlend_EaseInOut;

	CamManager->SetViewTarget(PlayerCamera, TransParams);
}

