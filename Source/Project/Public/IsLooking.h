// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTDecorator.h"
#include "IsLooking.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_API UIsLooking : public UBTDecorator
{
	GENERATED_BODY()

		virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

public:
	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector	ViewerKey;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector	TargetKey;

	UPROPERTY(EditAnywhere)
		float	Angle = 20.0f;
};
