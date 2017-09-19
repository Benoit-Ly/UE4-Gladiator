// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SceneComponent.h"
#include "FacingBar.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_API UFacingBar : public USceneComponent
{
	GENERATED_BODY()

public:	

	FVector DefaultScale;

	APlayerCameraManager * Camera;

	// Sets default values for this component's properties
	UFacingBar();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	void UpdateRangeBar(float value);

	void SetCamera(AController *controller);

	UPROPERTY(EditAnywhere)
	float AnimationLength = 1.0f;

	UPROPERTY(EditAnywhere)
	float ShakeAnimationOffet = 15.f;

	UPROPERTY(EditAnywhere)
	float ShakeAnimationSpeed = 50.f;

private :

	FVector DefaultLocation;

	float ValueToAim;
	float LastValue;

	float Timer;

	UMaterialInstanceDynamic *DynamicMat = nullptr;

	void ScaleToValue();
	void ShakeBar();
	
};
