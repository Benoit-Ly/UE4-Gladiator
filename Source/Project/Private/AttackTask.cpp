// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "AttackTask.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AICharacter.h"

UAttackTask::UAttackTask(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Attack Task";
	bNotifyTick = true;
}

EBTNodeResult::Type	UAttackTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AController		*Controller = Cast<AController>(OwnerComp.GetOwner());
	AAICharacter	*AI = (Controller != nullptr) ? Cast<AAICharacter>(Controller->GetPawn()) : nullptr;
	
	UBlackboardComponent	*BlackBoard = OwnerComp.GetBlackboardComponent();
	if (BlackBoard == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	bool IsAttacking = BlackBoard->GetValueAsBool("IsAttacking");
	if (!IsAttacking && AI->IsAttacking() == false)
	{
		BlackBoard->SetValueAsBool("IsAttacking", true);
		AI->TriggerAttack();
		return EBTNodeResult::InProgress;
	}
	return EBTNodeResult::Failed;
}

void UAttackTask::TickTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	AController		*Controller = Cast<AController>(OwnerComp.GetOwner());
	AAICharacter	*AI = (Controller != nullptr) ? Cast<AAICharacter>(Controller->GetPawn()) : nullptr;

	UBlackboardComponent	*BlackBoard = OwnerComp.GetBlackboardComponent();
	if (BlackBoard == nullptr)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}

	bool IsAttacking = BlackBoard->GetValueAsBool("IsAttacking");
	if (IsAttacking && AI->IsAttacking() == false)
	{
		BlackBoard->SetValueAsBool("IsAttacking", false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}


