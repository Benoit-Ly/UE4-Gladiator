// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "ArenaCharacter.h"
#include "ArenaFight_GameState.h"
#include "FXManager.h"
#include "FacingBar.h"
#include "PickableItemBase.h"
#include "PickableWeapon.h"
#include "UnrealNetwork.h"

// Sets default values
AArenaCharacter::AArenaCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void	AArenaCharacter::BeginPlay()
{
	Super::BeginPlay();

	DamageTimer = DamageLength;

	currentLife = maxLife;
	GetMesh()->SetRenderCustomDepth(true);

	{
		TArray<UFacingBar*> UFacingBars;
		GetComponents<UFacingBar>(UFacingBars);
		if (UFacingBars.Num() > 0)
		{
			HealthBar = UFacingBars[0];
			HealthBar->SetCamera(Controller);
		}
	}

	UMaterialInterface* Material = GetMesh()->GetMaterial(0);
	DynamicMat = UMaterialInstanceDynamic::Create(Material, this);
	DynamicMat->SetVectorParameterValue("FilterColor", DefaultDamageColorRGB);

	GetMesh()->SetMaterial(0, DynamicMat);
}

void	AArenaCharacter::InstantiateDefaultItems_Implementation()
{
	FActorSpawnParameters	Params;
	Params.bNoFail = true; // Triggers warning, but new version creates unexpected errors

	if (DefaultShieldClass)
	{
		APickableItemBase	*shield = Cast<APickableItemBase>(GetWorld()->SpawnActor(DefaultShieldClass, new FVector(), new FRotator(), Params));
		if (!shield)
		{
			LOG_TEMP(TEXT("AArenaCharacter::InstantiateDefaultItems_Implementation() - instantiated shield is NULL"));
		}
		PickShield(shield);
	}
	else
	{
		LOG_TEMP(TEXT("AArenaCharacter::InstantiateDefaultItems_Implementation() - DefaultShieldClass is NULL"));
	}

	if (DefaultWeaponClass)
	{
		APickableWeapon		*weapon = Cast<APickableWeapon>(GetWorld()->SpawnActor(DefaultWeaponClass, new FVector(), new FRotator(), Params));
		if (!weapon)
		{
			LOG_TEMP(TEXT("AArenaCharacter::InstantiateDefaultItems_Implementation() - instantiated weapon is NULL"));
		}
		PickWeapon(weapon);
	}
	else
	{
		LOG_TEMP(TEXT("AArenaCharacter::InstantiateDefaultItems_Implementation() - DefaultWeaponClass is NULL"));
	}

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AArenaCharacter::OnOverlapBegin);
}

bool	AArenaCharacter::InstantiateDefaultItems_Validate()
{
	return true;
}

void	AArenaCharacter::Tick( float DeltaTime )
{
	Super::Tick(DeltaTime);

	UpdateColor(DeltaTime);
}


#pragma region Character Stats

void	AArenaCharacter::LoseLife()
{
	if (Role != ROLE_Authority)
	{
		UE_LOG(LogSecurity, Warning, TEXT("AArenaCharacter::LoseLife() called with no authority!"))
		return;
	}

	DamageTimer = 0.0f;

	currentLife -= (ShieldActor ? 1 : 2);
	if (currentLife < 0)
		currentLife = 0;
	
	OnRep_ReplicatedLife();

	TryThrowWeapon();
	TryThrowShield();

	if (currentLife <= 0)
		OnDeath();
}

int		AArenaCharacter::GetMaxLife() const
{
	return maxLife;
}

int		AArenaCharacter::GetCurrentLife() const
{
	return currentLife;
}

void	AArenaCharacter::SetMaxLife(const int value)
{
	if (value > 0)
		maxLife = value;

	if (currentLife > maxLife)
		currentLife = maxLife;
}

bool	AArenaCharacter::IsDead() const
{
	return !bIsAlive;
}

void	AArenaCharacter::OnRep_ReplicatedLife()
{
	if (HealthBar)
		HealthBar->UpdateRangeBar((float)currentLife / (float)maxLife);
}

FString AArenaCharacter::GetCharacterName() const
{
	return characterName;
}

#pragma endregion


#pragma region Rendering

void	AArenaCharacter::SetOutline(const int value)
{
	GetMesh()->CustomDepthStencilValue = value;
}

bool	AArenaCharacter::IsAttacking() const
{
	return bIsAttacking;
}

void	AArenaCharacter::TriggerHit()
{
	if (Role == ROLE_Authority && WeaponCollider != nullptr)
	{
		TArray<AActor *> OverlappingActors;
		WeaponCollider->GetOverlappingActors(OverlappingActors, TSubclassOf<AArenaCharacter>());

		AActor	*self = Cast<AActor>(this);

		for (AActor *oneTarget : OverlappingActors)
		{
			if (oneTarget != self)
			{
				AArenaCharacter	*arenaCharacter = Cast<AArenaCharacter>(oneTarget);

				if (arenaCharacter)
				{
					arenaCharacter->RegisterAttacker(this);
					OnHitFX(arenaCharacter->GetActorLocation());
					arenaCharacter->LoseLife();
				}
			}
		}
	}
}

void	AArenaCharacter::RegisterAttacker(AArenaCharacter* Attacker)
{
	if (Attacker == nullptr)
		return;

	this->LastAttacker = Attacker;
}

bool	AArenaCharacter::OnHitFX_Validate(FVector Location)
{
	return true;
}

void	AArenaCharacter::OnHitFX_Implementation(FVector Location)
{
	UParticleSystem* DamageFX = UFXManager::GetInstance()->GetDamageEffect();
	if (DamageFX)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DamageFX, Location, FRotator::ZeroRotator, true);
	USoundBase* DamageSound = UFXManager::GetInstance()->GetDamageSound();
	if (DamageSound)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DamageSound, Location);
}

void	AArenaCharacter::Trace(FHitResult& OutHit, const FVector& Location) const
{
	FVector Start = Location;
	FVector End = Location;

	Start.Z += 20.0f;
	End.Z -= 20.0f;

	//Re-initialize hit info
	OutHit = FHitResult(ForceInit);

	FCollisionQueryParams TraceParams(FName(TEXT("Footprint trace")), true, this);
	TraceParams.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, TraceParams);
}

void	AArenaCharacter::FootDown(const UArrowComponent* FootArrow) const
{
	if (FootArrow == nullptr)
		return;

	FHitResult HitResult;
	FVector FootWorldPosition = FootArrow->GetComponentTransform().GetLocation();
	FVector Forward = FootArrow->GetForwardVector();

	Trace(HitResult, FootWorldPosition);

	if (!HitResult.bBlockingHit)
		return;

	UPhysicalMaterial* PhysMat = HitResult.PhysMaterial.Get();

	FQuat floorRot = FRotationMatrix::MakeFromZX(HitResult.Normal, Forward).ToQuat();
	FQuat offsetRot(FRotator(0.0f, 90.0f, 0.0f));
	FRotator Rotation = (floorRot * offsetRot).Rotator();

	UClass * Decal = UFXManager::GetInstance()->GetFootstepSandDecal();;

	if (PhysMat->SurfaceType == FOOTPRINT_SURFACE_Sand)
	{
		if (Decal)
			AActor* DecalInstance = GetWorld()->SpawnActor(Decal, &HitResult.Location, &Rotation);

		UParticleSystem* DamageFX = UFXManager::GetInstance()->GetFootstepSandEffect();

		if (DamageFX)
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DamageFX, HitResult.Location);
	}
}

void	AArenaCharacter::UpdateColor(const float DeltaTime)
{
	if (DamageTimer == DamageLength)
		return;

	DamageTimer = FMath::Clamp(DamageTimer + DeltaTime, 0.f, DamageLength);

	float factor = DamageTimer / DamageLength;

	SetColorStrength(1.f - factor);
}

bool	AArenaCharacter::SetColorStrength_Validate(const float value)
{
	return true;
}

void	AArenaCharacter::SetColorStrength_Implementation(const float value)
{
	if (DynamicMat)
		DynamicMat->SetScalarParameterValue("FilterColorStrength", value);
}

#pragma endregion


#pragma region	Target / Focus

void	AArenaCharacter::SetTarget(AActor* Target)
{
	if (TargetFocused == Target)
		return;

	TargetFocused = Target;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = true;
}

void	AArenaCharacter::UnsetTarget()
{
	if (TargetFocused == nullptr)
		return;
	
	TargetFocused = nullptr;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
}

#pragma endregion


#pragma region	Equipment

void	AArenaCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Role != ROLE_Authority)
		return;
}

void	AArenaCharacter::PickItem(AActor* OtherActor)
{
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		if (OtherActor == ShieldActor || OtherActor == WeaponActor)
			return;
		APickableItemBase	*pickedItem = Cast<APickableItemBase>(OtherActor);
		if (PickShield(pickedItem) || PickWeapon(pickedItem))
			UFXManager::GetInstance()->CallOnItemFar();
	}
}

bool	AArenaCharacter::PickShield(APickableItemBase *pickedItem)
{
	if (!pickedItem || pickedItem->IsUsed == true)
		return false;

	if (pickedItem->Type == ItemType::shield && !ShieldActor)
	{
		if (pickedItem->AttachItemTo(GetMesh(), TEXT("DualWeaponPoint")) == true)
		{
			ShieldActor = pickedItem;
			return true;
		}
	}

	return false;
}

bool	AArenaCharacter::PickWeapon(APickableItemBase *pickedItem)
{
	if (!pickedItem || pickedItem->IsUsed == true)
		return false;
	if (pickedItem->Type == ItemType::weapon && !WeaponActor)
	{
		APickableWeapon * pickedWeapon = Cast<APickableWeapon>(pickedItem);
		return PickWeapon(pickedWeapon);
	}
	return false;
}

bool	AArenaCharacter::PickWeapon(APickableWeapon *pickedWeapon)
{
	if (!pickedWeapon || pickedWeapon->IsUsed == true)
		return false;

	if (!WeaponActor)
	{
		if (pickedWeapon->AttachItemTo(GetMesh(), TEXT("WeaponPoint")) == true)
		{
			WeaponActor = pickedWeapon;
			WeaponCollider = WeaponActor->GetDamageBox();
			return true;
		}
	}
	return false;
}

void	AArenaCharacter::TryThrowWeapon()
{
	if (Role != ROLE_Authority || !WeaponActor)
		return;
	float rand = FMath::FRand();
	if (rand < LoseWeaponProb)
	{
		WeaponActor->DetachItem();
		WeaponActor = nullptr;
		WeaponCollider = nullptr;
	}
}

void	AArenaCharacter::TryThrowShield()
{
	if (Role != ROLE_Authority || !ShieldActor)
		return;
	float rand = FMath::FRand();
	if (rand < LoseShieldProb)
	{
		ShieldActor->DetachItem();
		ShieldActor = nullptr;
	}
}

bool	AArenaCharacter::HasWeapon()	const
{
	return WeaponActor != nullptr;
}

bool	AArenaCharacter::HasShield()	const
{
	return ShieldActor != nullptr;
}

#pragma endregion


#pragma region	RPC used for Networking

void	AArenaCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArenaCharacter, currentLife);
	DOREPLIFETIME(AArenaCharacter, bIsAttacking);
	DOREPLIFETIME(AArenaCharacter, bIsAlive);
	DOREPLIFETIME(AArenaCharacter, WeaponActor);
	DOREPLIFETIME(AArenaCharacter, ShieldActor);
	DOREPLIFETIME(AArenaCharacter, characterName);
}

/*
** Function used when calling OnDeath() - see .h
*/
void	AArenaCharacter::OnDeath_Implementation()
{
	bIsAlive = false;

	UnsetTarget();
	SetColorStrength(0);

	GetCharacterMovement()->DisableMovement();
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	if (this->LastAttacker != nullptr)
	{
		AArenaFight_GameState* currentGameState = (GetWorld() != nullptr ? GetWorld()->GetGameState<AArenaFight_GameState>() : nullptr);

		currentGameState->ReportCharacterKill(this->LastAttacker->GetCharacterName(), this->GetCharacterName());
	}
	
}

/*
** Function checking if allowed to call OnDeath()
*/
bool	AArenaCharacter::OnDeath_Validate()
{
	return true;
}

/*
** Function used when calling TriggerAttack() - see .h
*/
void	AArenaCharacter::TriggerAttack_Implementation()
{
	if (WeaponActor)
		Attack();
}

/*
** Function checking if allowed to call TriggerAttack()
*/
bool	AArenaCharacter::TriggerAttack_Validate()
{
	return true;
}

/*
** Function used when calling Attack() - see .h
*/
void	AArenaCharacter::Attack_Implementation()
{
	bIsAttacking = true;
}

/*
** Function checking if allowed to call Attack()
*/
bool	AArenaCharacter::Attack_Validate()
{
	return true;
}

#pragma endregion