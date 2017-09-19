// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTService.h" 
#include "AIAgroService.generated.h" 

/**
*
*/
UCLASS()
class PROJECT_API UAIAgroService : public UBTService
{
	GENERATED_UCLASS_BODY()

		virtual void TickNode(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere)
		float DefaultMinDistance = 100000.f;

	UPROPERTY(EditAnywhere)
		FBlackboardKeySelector  TargetKey;
private:

	float MinDistance;
	FVector CurrentLocation;
	int NearestPlayer;


};
