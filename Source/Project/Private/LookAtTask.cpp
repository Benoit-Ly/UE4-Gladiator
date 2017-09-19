// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "LookAtTask.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AICharacter.h"

EBTNodeResult::Type ULookAtTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AController		*Controller = Cast<AController>(OwnerComp.GetOwner());
	AAICharacter	*AI = (Controller != nullptr) ? Cast<AAICharacter>(Controller->GetPawn()) : nullptr;

	UBlackboardComponent	*BlackBoard = OwnerComp.GetBlackboardComponent();
	if (BlackBoard == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AActor	*Target = Cast<AActor>(BlackBoard->GetValueAsObject(TargetKey.SelectedKeyName));
	if (Target == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AI->SetTarget(Target);

	return EBTNodeResult::Succeeded;
}


