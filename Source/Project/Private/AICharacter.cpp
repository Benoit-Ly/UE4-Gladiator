// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "AICharacter.h"
#include "AIController.h"
#include "AIDirector.h"
#include "BehaviorTree/BlackboardComponent.h"

void	AAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
	{
		TActorIterator<AAIDirector> ActorItr(GetWorld());
		AIDirector = *ActorItr;
	}
	
	GetMesh()->SetCustomDepthStencilValue(0);

	AddItselfToAIDirector();
}

void	AAICharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Role != ROLE_Authority || OtherActor == nullptr)
		return;

	Super::OnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	this->PickItem(OtherActor);
}

void	AAICharacter::OnDeath_Implementation()
{
	Super::OnDeath_Implementation();

	UnsetTarget();

	NetMulticastNotifyDeath();
}

void	AAICharacter::NetMulticastNotifyDeath_Implementation()
{
	if (AIDirector != nullptr)
		AIDirector->NotifyAIDeath(this);
}

bool	AAICharacter::NetMulticastNotifyDeath_Validate()
{
	return true;
}


void	AAICharacter::SetTarget(AActor *Target)
{
	Super::SetTarget(Target);
	AAIController *aiController = Cast<AAIController>(GetController());
	if (aiController)
		aiController->SetFocus(Target);
}

UBlackboardComponent*	AAICharacter::GetBlackBoard()
{
	if (!AIBlackBoard)
	{
		AAIController*	AIController = Cast<AAIController>(GetController());
		if (AIController)
		{
			UBlackboardComponent* BlackBoard = AIController->FindComponentByClass<UBlackboardComponent>();
			if (BlackBoard)
				AIBlackBoard = BlackBoard;
		}
	}

	return AIBlackBoard;
}

void	AAICharacter::UnsetTarget()
{
	Super::UnsetTarget();

	AAIController *aiController = Cast<AAIController>(GetController());
	if (aiController)
		aiController->ClearFocus(EAIFocusPriority::LastFocusPriority);
}

void	AAICharacter::AddItselfToAIDirector_Implementation()
{
	if (AIDirector != nullptr)
		AIDirector->AddAICharacter(this);
	else
	{
		LOG_TEMP(TEXT("AAICharacter::AddItselfToAIDirector_Implementation() - AIDirector is nullptr"));
	}
}

bool	AAICharacter::AddItselfToAIDirector_Validate()
{
	return true;
}