// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "ArenaCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AICharacter.generated.h"

class	AAIDirector;

/**
 * Class used for every ArenaCharacter controlled by an AI
 */
UCLASS()
class PROJECT_API AAICharacter : public AArenaCharacter
{
	GENERATED_BODY()
	
public:
	// Called when the game starts or when spawned
	virtual void	BeginPlay() override;

	// Check Collision to pick up items automatically
	virtual void	OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	// Overriding OnDeath_Implementation() to notify specific death
	void	OnDeath_Implementation() override;

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void	NetMulticastNotifyDeath();

	UFUNCTION(Client, Reliable, WithValidation)
	void	AddItselfToAIDirector();

	virtual void	SetTarget(AActor *Target) override;
	virtual void	UnsetTarget()	override;
	UBlackboardComponent	*GetBlackBoard();

protected:
	// Pointer to the AIDirector managing this AICharacter
	AAIDirector		*AIDirector = nullptr;
	UBlackboardComponent	*AIBlackBoard;
};
