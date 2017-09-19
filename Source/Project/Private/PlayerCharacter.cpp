// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "PlayerCharacter.h"
#include "AIDirector.h"
#include "ArenaFight_GameState.h"
#include "ArenaFight_GameMode.h"
#include "ArenaFight_PlayerState.h"
#include "FXManager.h"
#include "UnrealNetwork.h"


#pragma region Overridden functions

void	APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	{
		TArray<USpringArmComponent *>	USpringArmComponents;
		GetComponents<USpringArmComponent>(USpringArmComponents);
		if (USpringArmComponents.Num() > 0)
			SpringArm = USpringArmComponents[0];
	}

	{
		TArray<UCameraComponent*> UCameraComponents;
		GetComponents<UCameraComponent>(UCameraComponents);
		if (UCameraComponents.Num() > 0)
			Camera = UCameraComponents[0];
	}

	{
		TActorIterator<AAIDirector> ActorItr(GetWorld());
		AIDirector = *ActorItr;
	}

	{
		TActorIterator<ACameraDirector> CamItr(GetWorld());
		CamDirector = *CamItr;
	}
	

	GetMesh()->SetCustomDepthStencilValue(252);
}

// Called every frame
void	APlayerCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	UpdateRotation(DeltaTime);
	UpdateCameraRotation(DeltaTime);

	if (TargetFocused)
	{
		AArenaCharacter * CharacterFocused = Cast<AArenaCharacter>(TargetFocused);
		if (CharacterFocused && CharacterFocused->IsDead())
			UnsetTarget();
	}
}

// Called to bind functionality to input
void	APlayerCharacter::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAxis("MoveHorizontal",	this, &APlayerCharacter::MoveHorizontal);
	InputComponent->BindAxis("MoveVertical",	this, &APlayerCharacter::MoveVertical);

	InputComponent->BindAxis("MouseHorizontal",	this, &APlayerCharacter::MouseHorizontal);
	InputComponent->BindAxis("MouseVertical",	this, &APlayerCharacter::MouseVertical);

	InputComponent->BindAction("Attack",	IE_Pressed, this, &APlayerCharacter::TriggerAttack);

	InputComponent->BindAction("Lock",		IE_Pressed, this, &APlayerCharacter::Lock);
	InputComponent->BindAction("LockNext",	IE_Pressed, this, &APlayerCharacter::LockNextAction);
	InputComponent->BindAction("LockLast",	IE_Pressed, this, &APlayerCharacter::LockLastAction);

	InputComponent->BindAction("PickItem",  IE_Pressed, this, &APlayerCharacter::TriggerPickItem);

	InputComponent->BindAction("RestartGame", IE_Pressed, this, &APlayerCharacter::Server_RestartGame);

	InputComponent->BindAction("DebugDie", IE_Pressed, this, &APlayerCharacter::OnDeath);

	InputComponent->BindAction("ReturnMainMenu", IE_Pressed, this, &APlayerCharacter::ReturnMainMenu);

	ConnectOverlap();
}

void	APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacter, GodMode);
}

FString	APlayerCharacter::GetCharacterName() const
{
	APlayerController* Controller = Cast<APlayerController>(GetController());
	AArenaFight_PlayerState* State = Cast<AArenaFight_PlayerState>(Controller->PlayerState);

	if (State)
		return State->PlayerName;

	return "NONAME";
}

#pragma region Character Stats
void	APlayerCharacter::LoseLife()
{
	if (!GodMode)
		Super::LoseLife();
}
#pragma endregion

#pragma region Target
void	APlayerCharacter::SetTarget(AActor* Target)
{
	if (Target == nullptr)
		return;

	if (!bIsAlive)
		return;

	if (TargetFocused != nullptr)
		SetTargetColor(0);

	Super::SetTarget(Target);

	SetTargetColor(254);
	UFXManager::GetInstance()->CallOnLockTarget();

	if (SpringArm)
		SpringArm->bUsePawnControlRotation = false;
}

void	APlayerCharacter::UnsetTarget()
{
	if (TargetFocused == nullptr)
		return;

	SetTargetColor(0);

	Super::UnsetTarget();

	IndexTarget = 0;
	UFXManager::GetInstance()->CallOnUnlockTarget();

	if (SpringArm)
	{
		FRotator OldRotation = SpringArm->RelativeRotation;

		SpringArm->bUsePawnControlRotation = true;

		GetController()->SetControlRotation(FRotator(OldRotation.Pitch, GetActorRotation().Yaw + OldRotation.Yaw, 0));
	}
}
#pragma endregion

void	APlayerCharacter::OnDeath_Implementation()
{
	Super::OnDeath_Implementation();

	AArenaFight_GameState	*currentGameState = (GetWorld() != NULL ? GetWorld()->GetGameState<AArenaFight_GameState>() : NULL);

	if (currentGameState != NULL)
		currentGameState->Server_OnPlayerDied(Cast<APlayerController>(GetController()));
}

#pragma endregion


#pragma region Input Functions

void	APlayerCharacter::ReturnMainMenu()
{
	UGameplayStatics::OpenLevel(GetWorld(), "MainMenu");
}

void	APlayerCharacter::MouseVertical(float Value)
{
	float Input = Value * 100.0f * GetWorld()->GetDeltaSeconds();
	AddControllerPitchInput(Input);
}

void	APlayerCharacter::MouseHorizontal(float Value)
{
	float Input = Value * 100.0f * GetWorld()->GetDeltaSeconds();
	AddControllerYawInput(Input);
}

void	APlayerCharacter::MoveVertical(float Value)
{
	if (!bIsAttacking)
	{
		FRotator CamRot = GetControlRotation();
		CamRot.Pitch = 0.0f;
		FVector MoveDir = CamRot.Vector();

		UCharacterMovementComponent	*CharacterMovement = GetCharacterMovement();

		CharacterMovement->AddInputVector(MoveDir * Value);
	}
}

void	APlayerCharacter::MoveHorizontal(float Value)
{
	if (!bIsAttacking)
	{
		FRotator CamRot = GetControlRotation();
		CamRot.Pitch = 0.0f;
		FVector MoveDir = CamRot.RotateVector(FVector::RightVector);

		UCharacterMovementComponent	*CharacterMovement = GetCharacterMovement();

		CharacterMovement->AddInputVector(MoveDir * Value);
	}
}

void	APlayerCharacter::Lock()
{
	if (TargetFocused != nullptr)
		UnsetTarget();
	else
	{
		IndexTarget = -1;
		LockNext(true);		
	}
}

void	APlayerCharacter::LockNextAction()
{
	LockNext();
}

void	APlayerCharacter::LockNext(bool force)
{
	if (AIDirector == nullptr)
		return;

	if (!force && !TargetFocused)
		return;

	int	nbEnemy = AIDirector->GetEnemyCount();
	if (nbEnemy == 0)
		return;

	IndexTarget = (IndexTarget + 1) % nbEnemy;
	ACharacter * NewTarget = AIDirector->GetAIByDistance(this, IndexTarget);

	if (TargetFocused == NewTarget)
		LockLast();
	else if (NewTarget)
		SetTarget(NewTarget);
	else
		UnsetTarget();
}

void	APlayerCharacter::LockLastAction()
{
	LockLast();
}

void	APlayerCharacter::LockLast(bool force)
{
	if (AIDirector == nullptr)
		return;

	if (!force && !TargetFocused)
		return;

	IndexTarget--;
	if (IndexTarget < 0)
		IndexTarget = AIDirector->GetEnemyCount() - 1;

	ACharacter * NewTarget = AIDirector->GetAIByDistance(this, IndexTarget);

	if (TargetFocused == NewTarget)
		LockLast();
	else if (NewTarget)
		SetTarget(NewTarget);
	else
		UnsetTarget();
}

void	APlayerCharacter::TriggerPickItem_Implementation()
{
	TArray<AActor*> OverlappedItems;
	GetCapsuleComponent()->GetOverlappingActors(OverlappedItems, TSubclassOf<APickableItemBase>());

	for (AActor* Item : OverlappedItems)
		this->PickItem(Item);
}

bool	APlayerCharacter::TriggerPickItem_Validate()
{
	return true;
}

void	APlayerCharacter::Server_RestartGame_Implementation()
{
	AArenaFight_GameState	*currentGameState = (GetWorld() != nullptr ? GetWorld()->GetGameState<AArenaFight_GameState>() : nullptr);

	if (currentGameState != nullptr)
	{
		currentGameState->Server_OnRestartRequest(Cast<APlayerController>(GetController()));
	}
}

bool	APlayerCharacter::Server_RestartGame_Validate()
{
	return true;
}

void	APlayerCharacter::CustomEnableInput_Implementation()
{
	APlayerController	*playerController = Cast<APlayerController>(GetController());

	if (playerController)
	{
		EnableInput(playerController);
	}
}

bool	APlayerCharacter::CustomEnableInput_Validate()
{
	return true;
}

void	APlayerCharacter::CustomDisableInput_Implementation()
{
	APlayerController	*playerController = Cast<APlayerController>(GetController());

	if (playerController)
	{
		DisableInput(playerController);
	}
}

bool	APlayerCharacter::CustomDisableInput_Validate()
{
	return true;
}

#pragma endregion


#pragma region	Target

void	APlayerCharacter::SetTargetColor(const int value) const
{
	if (TargetFocused == nullptr)
		return;

	ACharacter	*focusedCharacter = Cast<ACharacter>(TargetFocused);
	if (focusedCharacter)
		focusedCharacter->GetMesh()->SetCustomDepthStencilValue(value);
}

void	APlayerCharacter::UpdateRotation(float DeltaSeconds)
{
	if (TargetFocused == nullptr || bIsAttacking || !bIsAlive)
		return;

	FRotator	TargetRotation = (TargetFocused->GetActorLocation() - GetActorLocation()).Rotation();
	TargetRotation.Pitch = 0.0f;

	FRotator	SmoothRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaSeconds, RotateSpeed);
	SetActorRotation(SmoothRotation);

	if (GetController())
		GetController()->SetControlRotation(SmoothRotation);
	else 
		UE_LOG(LogTemp, Warning, TEXT("Pas de controller"));
}

void	APlayerCharacter::UpdateCameraRotation(float DeltaSeconds)
{
	if (TargetFocused == nullptr || SpringArm == nullptr)
		return;

	FVector	RelTargetPoint = TargetFocused->GetActorLocation() - GetActorLocation();
	float	RelTargetLength = RelTargetPoint.Size();
	float	PercentDist = RelTargetLength / (CamMaxDistance - CamMinDistance);

	float	NewPitch = CamMinAngle + (1.0f - (FMath::Clamp(PercentDist, 0.0f, 1.0f))) * (CamMaxAngle - CamMinAngle);;
	FVector	Velocity = GetMesh()->K2_GetComponentRotation().UnrotateVector(GetVelocity());

	FRotator	TargetRotation = FRotator(0, 0, 0);
	TargetRotation.Pitch = -NewPitch;

	TargetRotation.Yaw = FMath::Clamp(Velocity.X, -1.f, 1.f) * CamXAngle * -1.f;

	FRotator	SmoothRotation = FMath::RInterpTo(SpringArm->RelativeRotation, TargetRotation, DeltaSeconds, CamRotateSpeed);
	SpringArm->SetRelativeRotation(SmoothRotation);
}

#pragma endregion


#pragma region Equipment

void	APlayerCharacter::OnActorBeginOverlap(class UPrimitiveComponent* OverlappingComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APickableItemBase	*ItemAround = Cast<APickableItemBase>(OtherActor);

	if (ItemAround)
	{
		if (ItemAround->IsUsed == false && ItemAround->CanBePicked())
		
			if ((ItemAround->Type == ItemType::shield && !HasShield()) || ItemAround->Type == ItemType::weapon && !HasWeapon())
			{
				ItemAround->Mesh->SetCustomDepthStencilValue(253);
				UFXManager::GetInstance()->CallOnItemNear();
			}
	}
}

void	APlayerCharacter::OnActorEndOverlap(class UPrimitiveComponent* OverlappingComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APickableItemBase	*ItemAround = Cast<APickableItemBase>(OtherActor);

	if (ItemAround)
	{
		if (ItemAround->IsUsed == false)
		{
			ItemAround->Mesh->SetCustomDepthStencilValue(255);
			UFXManager::GetInstance()->CallOnItemFar();
		}
	}
}

#pragma endregion


#pragma region Cheating

void	APlayerCharacter::SetGodMode_Implementation(const bool bEnable)
{
	GodMode = bEnable;
	if (GodMode)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("[%s] - GodMode activated"), *characterName));
		LOG_TEMP(TEXT("[%s] - GodMode activated"), *characterName);
	}
	else
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("[%s] - GodMode deactivated"), *characterName));
		LOG_TEMP(TEXT("[%s] - GodMode deactivated"), *characterName);
	}
}

bool	APlayerCharacter::SetGodMode_Validate(const bool bEnable)
{
	return true;
}

void	APlayerCharacter::OCanada()
{
	if (!GodMode)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Activating GodMode")));
		LOG_TEMP(TEXT("Activating GodMode"));
	}
	else
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Deactivating GodMode")));
		LOG_TEMP(TEXT("Deactivating GodMode"));
	}

	SetGodMode(!GodMode);
}

#pragma endregion


void APlayerCharacter::ConnectOverlap()
{
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnActorBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnActorEndOverlap);
}



void	APlayerCharacter::UseMatinee_Implementation(const bool bEnable)
{
	if (Camera && CamDirector)
	{
		if (bEnable)
			CamDirector->SwitchToMatinee();
		else 
			CamDirector->SwitchToPlayerCamera();
	}
}

bool	APlayerCharacter::UseMatinee_Validate(const bool bEnable)
{
	return true;
}