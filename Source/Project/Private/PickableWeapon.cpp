// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "PickableWeapon.h"

APickableWeapon::APickableWeapon()
	: APickableItemBase()
{
	Type = ItemType::weapon;
}

// Called when the game starts or when spawned
void	APickableWeapon::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void	APickableWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

UBoxComponent	*APickableWeapon::GetDamageBox() const
{
	return DamageBox;
}