// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTDecorator.h"
#include "DecoratorDistance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_API UDecoratorDistance : public UBTDecorator
{
	GENERATED_BODY()

		virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

public:
	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector	TargetKey;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector	DistanceKey;
	UPROPERTY(EditAnywhere)
		float	AcceptanceRadius;
};
