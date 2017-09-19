// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "FacingBar.h"
#include "Engine.h"


// Sets default values for this component's properties
UFacingBar::UFacingBar()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UFacingBar::BeginPlay()
{
	Super::BeginPlay();

	DefaultScale = GetComponentScale();
	DefaultLocation = RelativeLocation;

	Timer = AnimationLength;
	LastValue = 1;
	ValueToAim = 1;

	UStaticMeshComponent * StaticMesh = Cast < UStaticMeshComponent>(GetChildComponent(0));
	if (StaticMesh)
	{
		DynamicMat = UMaterialInstanceDynamic::Create(StaticMesh->GetMaterial(0), this);
		StaticMesh->SetMaterial(0, DynamicMat);
	}

	if (GEngine)
	{
		UWorld	*world = GetWorld();
		if (world)
		{
			APlayerController	*firstLocalPlayerController = GEngine->GetFirstLocalPlayerController(world);
			if (firstLocalPlayerController)
				Camera = firstLocalPlayerController->PlayerCameraManager;
		}
	}	
}


// Called every frame
void UFacingBar::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (Camera != nullptr)
	{
		FRotator camRot = Camera->GetCameraRotation();
		SetWorldRotation(camRot);
	}

	if (Timer != AnimationLength)
	{
		Timer = FMath::Clamp(Timer + DeltaTime, 0.f, AnimationLength);
		ScaleToValue();
		ShakeBar();
		if (Timer == AnimationLength)
		{
			LastValue = ValueToAim;
			if (LastValue <= 0.f)
				SetVisibility(false, true);
		}
	}
	
}

void UFacingBar::UpdateRangeBar(float value)
{
	LastValue = ValueToAim;
	ValueToAim = value;
	Timer = 0.f;
}

void UFacingBar::ScaleToValue()
{
	if (LastValue == ValueToAim)
		return;
	if (Timer == AnimationLength)
		return;
	float NewValue = FMath::InterpExpoOut(LastValue, ValueToAim, Timer/AnimationLength);
	SetWorldScale3D(FVector(DefaultScale.X, NewValue * DefaultScale.Y, DefaultScale.Z));

	if (DynamicMat)
		DynamicMat->SetScalarParameterValue("Alpha", NewValue);
}

void UFacingBar::ShakeBar()
{
	if (Timer == AnimationLength)
		return;
	float factor = Timer / AnimationLength;
	float result = ShakeAnimationOffet * (1 - factor) * FMath::Cos(Timer * ShakeAnimationSpeed);
	FVector Movement = DefaultLocation + FVector(0, 0, result);
	SetRelativeLocation(Movement);
}

void UFacingBar::SetCamera(AController *controller)
{
	APlayerController	*playerController = Cast<APlayerController>(controller);

	if (playerController)
		Camera = playerController->PlayerCameraManager;
}

