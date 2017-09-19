// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "DecoratorDistance.h"
#include "BehaviorTree/BlackboardComponent.h"


bool UDecoratorDistance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AController* Controller = Cast<AController>(OwnerComp.GetOwner());
	AActor* AI = (Controller != nullptr) ? Controller->GetPawn() : nullptr;
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if (BlackBoard == nullptr || AI == nullptr)
	{
		return false;
	}

	AActor* Target = Cast<AActor>(BlackBoard->GetValueAsObject(TargetKey.SelectedKeyName));
	if (Target == nullptr)
	{
		return false;
	}

	float Distance = BlackBoard->GetValueAsFloat(DistanceKey.SelectedKeyName);

	return (Target->GetDistanceTo(AI) <= Distance + AcceptanceRadius && 
		Target->GetDistanceTo(AI) >= Distance - AcceptanceRadius);
}

