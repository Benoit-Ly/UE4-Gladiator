// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "AIDirector.h"
#include "AICharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ArenaFight_GameState.h"
#include "UnrealNetwork.h"

// Sets default values
AAIDirector::AAIDirector()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	bAlwaysRelevant = true;
}

// Called when the game starts or when spawned
void	AAIDirector::BeginPlay()
{
	Super::BeginPlay();

	if (Role != ROLE_Authority)
	{
		PrimaryActorTick.bCanEverTick = false;
		return;
	}
}

// Called every frame
void	AAIDirector::Tick(float DeltaTime)
{
	if (Role != ROLE_Authority)
	{
		return;
	}

	Super::Tick(DeltaTime);
}

void	AAIDirector::SpawnAIs(const int nbPlayers, const int nbWave)
{
	int	nbToSpawn = nbPlayers * (nbWave + 1);

	if (nbToSpawn == 0)
		return;

	if (Role != ROLE_Authority)
	{
		return;
	}

	if (AIClass == nullptr || SpawnPoints.Num() <= 0)
	{
		return;
	}

	AIPawns.Empty();

	ACharacter	*DefaultAI = AIClass->GetDefaultObject<ACharacter>();
	float offset = DefaultAI->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	for (int i = 0; i < nbToSpawn; ++i)
	{
		int SpawnIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);
		FVector		location = SpawnPoints[SpawnIndex]->GetActorLocation();
		location.Z += offset;

		FRotator	rotation = SpawnPoints[SpawnIndex]->GetActorRotation();
		
		SpawnNewAICharacter(location, rotation);
	}

	AArenaFight_GameState* currentGameState = (GetWorld() != NULL ? GetWorld()->GetGameState<AArenaFight_GameState>() : NULL);

	if (currentGameState != NULL)
		currentGameState->ReportAISpawned();

	UpdateGotoPlayer();
}

AAICharacter	*AAIDirector::SpawnNewAICharacter(const FVector &location, const FRotator &rotation)
{
	if (Role != ROLE_Authority)
		return nullptr;

	FActorSpawnParameters	Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AAICharacter	*aiCharacter = Cast<AAICharacter>(GetWorld()->SpawnActor(AIClass, &location, &rotation, Params));
	if (aiCharacter == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AAIdirector::SpawnNewAICharacter() - couldn't spawn new AICharacter"));
		return nullptr;
	}
	aiCharacter->SpawnDefaultController();
	aiCharacter->InstantiateDefaultItems();

	InitAI(aiCharacter);

	return aiCharacter;
}

void	AAIDirector::InitAI(AAICharacter *aiCharacter)
{
	if (Role != ROLE_Authority || aiCharacter == nullptr)
	{
		return;
	}

	aiCharacter->SetMaxLife(AILife);

	UBlackboardComponent* BlackBoard = aiCharacter->GetBlackBoard();
	if (BlackBoard == nullptr)
	{
		return;
	}

	BlackBoard->SetValueAsObject("Player", UGameplayStatics::GetPlayerPawn(this, 0));
	BlackBoard->SetValueAsFloat("Distance", DistanceWithPlayer);
	BlackBoard->SetValueAsBool("GotoPlayer", false);
	BlackBoard->SetValueAsBool("CanMove", PlayAtStartup);
}

void	AAIDirector::AddAICharacter(AAICharacter *aiCharacter)
{
	AIPawns.AddUnique(aiCharacter);
}

int		AAIDirector::GetAliveEnemiesCount() const
{
	return AIPawns.Num();
}

void	AAIDirector::UpdateGotoPlayer()
{
	if (Role != ROLE_Authority)
	{
		return;
	}

	for (int32 i = 0; i < AIPawns.Num(); ++i)
	{
		AAICharacter* AI = AIPawns[i];
		if (AI == nullptr)
		{
			continue;
		}

		UBlackboardComponent* BlackBoard = AI->GetBlackBoard();
		if (BlackBoard == nullptr)
		{
			continue;
		}

		BlackBoard->SetValueAsBool("GotoPlayer", FMath::RandBool());
	}

	if (AIPawns.Num() > 0)
		CurrentAI = (CurrentAI + 1) % AIPawns.Num();
	GetWorld()->GetTimerManager().SetTimer(GotoPlayerTimerHandle, this, &AAIDirector::UpdateGotoPlayer, 1.0f, false, GotoPlayerDelay);
}

void AAIDirector::AIsSetEnabled(bool value)
{
	for (int32 i = 0; i < AIPawns.Num(); ++i)
	{
		AAICharacter* AI = AIPawns[i];
		if (AI == nullptr)
		{
			continue;
		}

		UBlackboardComponent* BlackBoard = AI->GetBlackBoard();
		if (BlackBoard == nullptr)
		{
			continue;
		}

		BlackBoard->SetValueAsBool("CanMove", value);
	}
}

ACharacter	*AAIDirector::GetEnemy(int index) const
{
	if (index < 0 || index >= AIPawns.Num())
		return nullptr;
	return AIPawns[index];
}

int AAIDirector::GetEnemyCount() const
{
	return AIPawns.Num();
}

void AAIDirector::NotifyAIDeath(AAICharacter * DeadAI)
{
	int index = AIPawns.Find(DeadAI);
	if (index >= 0)
	{
		AIPawns.RemoveAtSwap(index);
	}
	else
		return;

	if (AIPawns.Num() == 0)
	{
		AArenaFight_GameState* currentGameState = (GetWorld() != NULL ? GetWorld()->GetGameState<AArenaFight_GameState>() : NULL);

		if (currentGameState != NULL)
			currentGameState->Server_OnAIDirectorDied(this);
	}
}

int AAIDirector::GetIndexNearestAI(AActor * Target)
{
	float MinDistance = 100000;
	int32 SelectedIndex = -1;
	for (int32 i = 0; i < AIPawns.Num(); ++i)
	{
		AAICharacter* AI = AIPawns[i];
		if (AI == nullptr)
		{
			continue;
		}

		float Distance = (Target->GetActorLocation() - AI->GetActorLocation()).Size();
		if (Distance < MinDistance)
		{
			SelectedIndex = i;
			MinDistance = Distance;
		}
	}

	return SelectedIndex;
}
ACharacter * AAIDirector::GetAIByDistance(AActor * Target, int index)
{
	if (index < 0 || index >= AIPawns.Num())
		return nullptr;

	TArray<AAICharacter*> tmpAIPawns(AIPawns);

	for (int i = 0; i < tmpAIPawns.Num() - 1; i++)
		for (int j = i + 1; j < tmpAIPawns.Num(); j++)
			if (Target->GetDistanceTo(tmpAIPawns[i]) > Target->GetDistanceTo(tmpAIPawns[j]))
				tmpAIPawns.Swap(i, j);
	
	return tmpAIPawns[index];
}
