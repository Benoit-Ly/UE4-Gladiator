// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTDecorator.h"
#include "CanLookAtDecorator.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_API UCanLookAtDecorator : public UBTDecorator
{
	GENERATED_UCLASS_BODY()

		virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	void Trace(FHitResult& OutHit, TArray<AActor *> IgnoredActors, const AActor * From, const AActor * Target) const;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector	TargetKey;
	UPROPERTY(EditAnywhere)
		float	AngleVision;

};
