// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "ArenaCharacter.h"
#include "HasWeaponDecorator.h"

bool UHasWeaponDecorator::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AController* Controller = Cast<AController>(OwnerComp.GetOwner());
	AArenaCharacter	*AI = (Controller != nullptr) ? Cast<AArenaCharacter>(Controller->GetPawn()) : nullptr;

	return AI->HasWeapon();
}


