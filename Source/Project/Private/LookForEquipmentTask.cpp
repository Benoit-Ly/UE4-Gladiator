// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "ArenaCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LookForEquipmentTask.h"



ULookForEquipmentTask::ULookForEquipmentTask(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Look For Equipment";
	bNotifyTick = true;
}

EBTNodeResult::Type	ULookForEquipmentTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::InProgress;
}

void ULookForEquipmentTask::TickTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	AController		*Controller = Cast<AController>(OwnerComp.GetOwner());
	AArenaCharacter	*AI = (Controller != nullptr) ? Cast<AArenaCharacter>(Controller->GetPawn()) : nullptr;

	if (EquipmentType == ItemType::shield && AI->HasShield())
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	if (EquipmentType == ItemType::weapon && AI->HasWeapon())
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	FVector ForwardVector = AI->GetActorRotation().Vector();

	APickableItemBase * SeenItem = nullptr;
	for (TActorIterator<APickableItemBase> ActorItr(OwnerComp.GetOwner()->GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		APickableItemBase *Item = *ActorItr;
		if (Item->IsUsed)
			continue;
		FVector ToItem = (Item->GetActorLocation() - AI->GetActorLocation());
		ToItem.Normalize();
		float CurrentAngle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(ForwardVector, ToItem)));
		if (CurrentAngle < AngleVision && EquipmentType == Item->Type)
		{
			SeenItem = Item;
			break;
		}
	}
	if (SeenItem == nullptr)
		return;

	UBlackboardComponent	*BlackBoard = OwnerComp.GetBlackboardComponent();
	if (BlackBoard == nullptr)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	BlackBoard->SetValueAsObject(TargetKey.SelectedKeyName, SeenItem);
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}
