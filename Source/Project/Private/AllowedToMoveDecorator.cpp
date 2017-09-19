// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "AllowedToMoveDecorator.h"
#include "BehaviorTree/BlackboardComponent.h"

bool UAllowedToMoveDecorator::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if (BlackBoard == nullptr)
	{
		return false;
	}

	return BlackBoard->GetValueAsBool(BoolKey.SelectedKeyName);

}


