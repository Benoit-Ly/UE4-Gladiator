// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PickableItemBase.h"
#include "GameFramework/Actor.h"
#include "PickableWeapon.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_API APickableWeapon : public APickableItemBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APickableWeapon();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UBoxComponent	*GetDamageBox() const;

protected:
	UPROPERTY(Category = "Inherited|PickableWeapon", BlueprintReadWrite, VisibleAnywhere)
	UBoxComponent	*DamageBox = nullptr;
};
