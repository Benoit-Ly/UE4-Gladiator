// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "LookAtTask.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_API ULookAtTask : public UBTTaskNode
{
	GENERATED_BODY()
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector	TargetKey;

	UPROPERTY(EditAnywhere)
		float	Duration = 1.0f;
	
	
};
