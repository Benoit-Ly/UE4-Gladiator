// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaCharacter.h"
#include "PickableItemBase.h"
#include "CameraDirector.h"
#include "PlayerCharacter.generated.h"

class	AAIDirector;

/**
* Class used for every ArenaCharacter controlled by a Player
*/
UCLASS()
class PROJECT_API APlayerCharacter : public AArenaCharacter
{
	GENERATED_BODY()
		
#pragma region Overridden functions
public:
	// Called when the game starts or when spawned
	virtual void	BeginPlay() override;

	// Called every frame
	virtual void	Tick(float DeltaSeconds) override;

	// Called to bind functionality to input
	virtual void	SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	virtual void	GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual FString	GetCharacterName() const override;

#pragma region Character Stats
	// GodMode Implementation
	virtual void	LoseLife() override;
#pragma endregion

#pragma region Target
	virtual void	SetTarget(AActor* Target) override;
	virtual void	UnsetTarget() override;
#pragma endregion

	// Overriding OnDeath_Implementation() to notify specific death
	virtual void	OnDeath_Implementation() override;
#pragma endregion

protected:
	// Pointer to the AIDirector managing every AICharacter
	AAIDirector			*AIDirector = nullptr;

	USpringArmComponent	*SpringArm = nullptr;
	UCameraComponent	*Camera = nullptr;

#pragma region Input Functions
public:
	UFUNCTION(Client, Reliable, WithValidation, Category = "RPC function|Client")
	virtual void	CustomEnableInput();

	UFUNCTION(Client, Reliable, WithValidation, Category = "RPC function|Client")
	virtual void	CustomDisableInput();

protected:
	void	ReturnMainMenu();

	void	MoveHorizontal(float value);
	void	MoveVertical(float value);
	void	MouseHorizontal(float value);
	void	MouseVertical(float value);

	void	Lock();
	void	LockLastAction();
	void	LockNextAction();
	void	LockLast(bool force = false);
	void	LockNext(bool force = false);

	UFUNCTION(Server, Unreliable, WithValidation, Category = "RPC function|Server")
	void	TriggerPickItem();

	UFUNCTION(Server, Reliable, WithValidation, Category = "RPC function|Server")
	void	Server_RestartGame();
#pragma endregion

#pragma region Character Stats
protected:
	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Debug", meta = (DisplayName = "Enable God Mode"))
	bool	GodMode = false;
#pragma endregion
	
#pragma region Target
protected:
	void	SetTargetColor(const int value) const;

	void	UpdateRotation(float DeltaSeconds);
	void	UpdateCameraRotation(float DeltaSeconds);

	int		IndexTarget = -1;
	UPROPERTY(Category = "Camera", EditAnywhere)
	float	CamRotateSpeed = 60.f;
	UPROPERTY(Category = "Camera", EditAnywhere)
	float	CamXAngle = 30.f;
	UPROPERTY(Category = "Camera", EditAnywhere)
	float	CamMaxAngle = 80;
	UPROPERTY(Category = "Camera", EditAnywhere)
	float	CamMinAngle = 10;
	UPROPERTY(Category = "Camera", EditAnywhere)
	float	CamMaxDistance = 700;
	UPROPERTY(Category = "Camera", EditAnywhere)
	float	CamMinDistance = 0;
	UPROPERTY(Category = "Camera", EditAnywhere, BlueprintReadWrite)
	float	RotateSpeed = 80.f;
#pragma endregion

#pragma region Equipment
private:
	void ConnectOverlap();
	UFUNCTION()
	void OnActorBeginOverlap(class UPrimitiveComponent* OverlappingComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnActorEndOverlap(class UPrimitiveComponent* OverlappingComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
#pragma endregion

#pragma region Cheating
public:
	UFUNCTION(Exec)
	void	OCanada();
private:
	UFUNCTION(Server, Unreliable, WithValidation, Category = "RPC function|Server")
	void	SetGodMode(const bool bEnable);
#pragma endregion

public:
	UFUNCTION(Client, Reliable, WithValidation)
	void	UseMatinee(const bool bEnable);

	ACameraDirector * CamDirector;

};
