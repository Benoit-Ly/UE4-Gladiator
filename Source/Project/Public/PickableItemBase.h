// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PickableItemBase.generated.h"

UENUM(BlueprintType)
enum class ItemType : uint8
{
	weapon	UMETA(DisplayName = "Weapon")
	, shield	UMETA(DisplayName = "Shield")
	, none		UMETA(DisplayName = "None")
};

UCLASS()
class PROJECT_API APickableItemBase : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inherited)
	UStaticMeshComponent	*StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inherited)
	USkeletalMeshComponent	*Mesh;

	//add collider for NPC
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Inherited)
	USphereComponent		*ProxSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickable Item")
	ItemType	Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickable Item")
	float		TimeToPickIt = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickable Item|Default Primitive")
	FVector		DefaultStaticMeshLocation = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickable Item|Default Primitive")
	FRotator	DefaultStaticMeshRotation = FRotator::ZeroRotator;

	float CannotPickTimer = 0.0f;

	int CurrentRenderingColor = 0;

	// Sets default values for this actor's properties
	APickableItemBase();

	// Called when the game starts or when spawned
	virtual void	BeginPlay() override;
	
	// Called every frame
	virtual void	Tick( float DeltaSeconds ) override;

	virtual void	GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated, VisibleAnywhere, Category = "Pickable Item")
	bool	IsUsed = false;

	bool	AttachItemTo(USkeletalMeshComponent *meshRoot, FName socket);
	void	DetachItem();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void	ChangeRenderingState(const bool bSimulatePhysics, const ECollisionEnabled::Type collisionType, const int depthStencilValue);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void	ChangeRenderingColor(const int depthStencilValue);

	bool CanBePicked();
};
