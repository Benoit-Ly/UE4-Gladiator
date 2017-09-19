// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "PickableItemBase.h"
#include "LookForEquipmentTask.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_API ULookForEquipmentTask : public UBTTaskNode
{
	GENERATED_UCLASS_BODY()
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	UPROPERTY(EditAnywhere)
	ItemType EquipmentType;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector	TargetKey;

	UPROPERTY(EditAnywhere)
		float AngleVision = 10.f;
	
};
