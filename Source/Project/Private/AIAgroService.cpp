// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "AIAgroService.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "ArenaCharacter.h"

UAIAgroService::UAIAgroService(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCallTickOnSearchStart = true;
	NodeName = "AI Agro Service";

}

void UAIAgroService::TickNode(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	int NumPlayers = UGameplayStatics::GetGameMode(OwnerComp.GetOwner()->GetWorld())->GetNumPlayers();

	if (NumPlayers == 0)
		return;

	AAIController		*Controller = Cast<AAIController>(OwnerComp.GetOwner());
	AArenaCharacter	*AI = (Controller != nullptr) ? Cast<AArenaCharacter>(Controller->GetPawn()) : nullptr;

	{
		NearestPlayer = -1;
		MinDistance = DefaultMinDistance;
		CurrentLocation = AI->GetActorLocation();
	}

	AArenaCharacter * NewTarget = nullptr;

	for (int i = 0; i < NumPlayers; i++)
	{
		AArenaCharacter *CurrPlayer = Cast<AArenaCharacter>(UGameplayStatics::GetPlayerPawn(Controller->GetWorld(), i));
		if (!CurrPlayer)
			continue;
		float Distance = CurrPlayer->GetDistanceTo(AI);
		if (Distance < MinDistance && CurrPlayer->IsDead() == false)
		{
			NearestPlayer = i;
			MinDistance = Distance;
			NewTarget = CurrPlayer;
		}
	}

	UBlackboardComponent	*BlackBoard = OwnerComp.GetBlackboardComponent();
	if (BlackBoard == nullptr)
	{
		return;
	}

	if (NearestPlayer < 0)
	{
		AI->UnsetTarget();
		BlackBoard->SetValueAsObject(TargetKey.SelectedKeyName, nullptr);
	}
	else
	{
		AI->SetTarget(NewTarget);
		BlackBoard->SetValueAsObject(TargetKey.SelectedKeyName, NewTarget);
		Controller->SetFocus(NewTarget);
	}
}
