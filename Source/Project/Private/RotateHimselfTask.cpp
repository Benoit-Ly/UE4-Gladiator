// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "RotateHimselfTask.h"
#include "ArenaCharacter.h"

URotateHimselfTask::URotateHimselfTask(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Rotate";
	bNotifyTick = true;
}

EBTNodeResult::Type	URotateHimselfTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Timer = 0;
	return EBTNodeResult::InProgress;
}

void URotateHimselfTask::TickTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	AController		*Controller = Cast<AController>(OwnerComp.GetOwner());
	AArenaCharacter	*AI = (Controller != nullptr) ? Cast<AArenaCharacter>(Controller->GetPawn()) : nullptr;

	if (AI == nullptr)
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

	AI->AddActorWorldRotation(FRotator(0,(DeltaSeconds / Duration) * Angle, 0));
	Timer += DeltaSeconds;

	if (Timer >= Duration)
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}


