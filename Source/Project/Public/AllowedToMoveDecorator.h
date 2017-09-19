// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTDecorator.h"
#include "AllowedToMoveDecorator.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_API UAllowedToMoveDecorator : public UBTDecorator
{
	GENERATED_BODY()
	
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

public:
	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector	BoolKey;
	
	
};
