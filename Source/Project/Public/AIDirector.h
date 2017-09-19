// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "AIDirector.generated.h"

class	AAICharacter;

UCLASS()
class PROJECT_API AAIDirector : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(Category = "AI", BlueprintReadWrite, EditAnywhere)
	UClass	*AIClass;

	UPROPERTY(Category = "AI", BlueprintReadWrite, EditAnywhere)
	TArray<AActor*>	SpawnPoints;

	UPROPERTY(Category = "AI", BlueprintReadWrite, EditAnywhere)
	float	GotoPlayerDelay = 2.0f;

	UPROPERTY(Category = "AI", BlueprintReadWrite, EditAnywhere)
	float	DistanceWithPlayer = 200.f;

	UPROPERTY(Category = "AI", BlueprintReadWrite, EditAnywhere)
	int		AILife = 3;

	UPROPERTY(Category = "AI", BlueprintReadWrite, EditAnywhere)
		bool		PlayAtStartup = true;


public:
	// Sets default values for this actor's properties
	AAIDirector();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	ACharacter	*GetEnemy(int index) const;
	UFUNCTION(BlueprintCallable, Category = "AI")
	int		GetEnemyCount() const;

	void	NotifyAIDeath(AAICharacter * DeadAI);

	UFUNCTION(Category = "AI", BlueprintCallable)
	void	SpawnAIs(const int nbPlayers, const int nbWave);

	AAICharacter	*SpawnNewAICharacter(const FVector &location, const FRotator &rotation);

	void	AddAICharacter(AAICharacter *aiCharacter);

	ACharacter *		GetAIByDistance(AActor * Target, int index);
	UFUNCTION(BlueprintCallable, Category = "AI")
	int		GetAliveEnemiesCount() const;

	void AIsSetEnabled(bool value);


private:
	void	InitAI(AAICharacter	*aiCharacter);

	void	UpdateGotoPlayer();

	int		GetIndexNearestAI(AActor * Target);
	
	TArray<AAICharacter*>	AIPawns;

	int32			CurrentAI = 0;
	FTimerHandle	GotoPlayerTimerHandle;

	int NumberAIsAlive = 0;
};

