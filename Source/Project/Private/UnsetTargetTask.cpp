// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "ArenaCharacter.h"
#include "UnsetTargetTask.h"
#include "AIController.h"


EBTNodeResult::Type	UUnsetTargetTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController		*Controller = Cast<AAIController>(OwnerComp.GetOwner());
	AArenaCharacter	*AI = (Controller != nullptr) ? Cast<AArenaCharacter>(Controller->GetPawn()) : nullptr;
	AI->UnsetTarget();

	return EBTNodeResult::Succeeded;
}

