// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "RotateHimselfTask.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_API URotateHimselfTask : public UBTTaskNode
{
	GENERATED_UCLASS_BODY()
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere)
		float Angle;

	UPROPERTY(EditAnywhere)
		float Duration;

	float Timer;
};
