// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "PickableItemBase.h"
#include "UnrealNetwork.h"

// Sets default values
APickableItemBase::APickableItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	IsUsed = false;

	bReplicates = true;
	bAlwaysRelevant = true;
	bNetLoadOnClient = false;
	bReplicateMovement = true;
}

// Called when the game starts or when spawned
void	APickableItemBase::BeginPlay()
{
	Super::BeginPlay();

	if (Mesh)
		Mesh->SetRenderCustomDepth(true);
	
	ChangeRenderingState(false, ECollisionEnabled::Type::NoCollision, 0);

	if (Role != ROLE_Authority)
	{
		PrimaryActorTick.bCanEverTick = false;
	}
}

// Called every frame
void	APickableItemBase::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	CannotPickTimer = FMath::Clamp(CannotPickTimer - DeltaTime, 0.0f, TimeToPickIt);

	if (CannotPickTimer != 0.f)
	{
		int NewColor = (int)(CannotPickTimer / .2f) % 2 == 0 ? 254 : 0;
		if (NewColor != CurrentRenderingColor)
			ChangeRenderingColor(NewColor);
	}
	else if (CannotPickTimer == 0.f && CurrentRenderingColor != 255 && !IsUsed)
	{
		ChangeRenderingColor(255);
	}
}

void	APickableItemBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APickableItemBase, IsUsed);
}

bool	APickableItemBase::AttachItemTo(USkeletalMeshComponent *meshRoot, FName socket)
{
	if (IsUsed || CannotPickTimer > 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("APickableitemBase::AttachItemTo(): failed"));
		return false;
	}

	IsUsed = true;

	ChangeRenderingState(false, ECollisionEnabled::Type::NoCollision, 0);
	AttachToComponent(meshRoot, FAttachmentTransformRules::SnapToTargetNotIncludingScale, socket);

	SetActorRelativeLocation(FVector::ZeroVector);
	SetActorRelativeRotation(FRotator::ZeroRotator);

	if (StaticMesh)
	{
		StaticMesh->SetRelativeLocation(DefaultStaticMeshLocation);
		StaticMesh->SetRelativeRotation(DefaultStaticMeshRotation);
	}
	
	return true;
}

void	APickableItemBase::DetachItem()
{
	if (!IsUsed)
	{
		UE_LOG(LogTemp, Warning, TEXT("APickableitemBase::DetachItem(): failed"));
		return;
	}

	IsUsed = false;
	CannotPickTimer = TimeToPickIt;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	ChangeRenderingState(true, ECollisionEnabled::Type::QueryAndPhysics, 255);

	if (StaticMesh)
	{
		StaticMesh->AddImpulse(FVector(100, 0, 0));
	}
}

void	APickableItemBase::ChangeRenderingState_Implementation(const bool bSimulatePhysics, const ECollisionEnabled::Type collisionType, const int depthStencilValue)
{
	if (StaticMesh)
	{
		StaticMesh->SetSimulatePhysics(bSimulatePhysics);
		StaticMesh->SetCollisionEnabled(collisionType);
	}
	
	ChangeRenderingColor(depthStencilValue);
}

bool	APickableItemBase::ChangeRenderingState_Validate(const bool bSimulatePhysics, const ECollisionEnabled::Type collisionType, const int depthStencilValue)
{
	return true;
}

void APickableItemBase::ChangeRenderingColor_Implementation(const int depthStencilValue)
{
	if (Mesh)
		Mesh->SetCustomDepthStencilValue(depthStencilValue);

	CurrentRenderingColor = depthStencilValue;
}

bool APickableItemBase::ChangeRenderingColor_Validate(const int depthStencilValue)
{
	return true;
}

bool APickableItemBase::CanBePicked()
{
	return CannotPickTimer == 0.f;
}