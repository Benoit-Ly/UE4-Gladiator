// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CameraDirector.generated.h"

UCLASS()
class PROJECT_API ACameraDirector : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACameraDirector();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere)
	AActor * MatineeCamera;
	AActor * PlayerCamera;

	APlayerCameraManager * CamManager;

	FViewTargetTransitionParams TransParams;

	void SwitchToMatinee();
	void SwitchToPlayerCamera();
	
};
