// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "ArenaCharacter.generated.h"

class	UFacingBar;
class	APickableItemBase;
class	APickableWeapon;

/**
 * Base class for every Fighter in the Arena
 */
UCLASS(abstract)
class PROJECT_API AArenaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AArenaCharacter();
	
	// Called when the game starts or when spawned
	virtual void	BeginPlay() override;

	// Called on Server after Client has initialized himself
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Equipment")
	virtual void	InstantiateDefaultItems();

	// Called every frame
	virtual void	Tick(float DeltaSeconds) override;

protected:
	// Pointer on the actual weapon collider
	UBoxComponent	*WeaponCollider	= nullptr;

	// Pointer on the Character's healthBar
	UFacingBar		*HealthBar		= nullptr;

	
#pragma region	Character Stats
public:
	virtual void	LoseLife();
	virtual FString GetCharacterName() const;

	int		GetMaxLife() const;
	int		GetCurrentLife() const;

	void	SetMaxLife(const int value);

	bool	IsDead() const;

	// Function caled when currentLife gets replicated
	UFUNCTION()
	void	OnRep_ReplicatedLife();

protected:
	UPROPERTY(Category = "Character Stats", ReplicatedUsing = OnRep_ReplicatedLife, VisibleAnywhere)
	int		currentLife;

	UPROPERTY(Category = "Character Stats", EditDefaultsOnly)
	int		maxLife = 5;	

	UPROPERTY(Replicated, Category = "Character Stats", EditDefaultsOnly, BlueprintReadWrite)
	FString	characterName = "";
#pragma endregion


#pragma region	Rendering
public:
	void	SetOutline(const int value);

	bool	IsAttacking() const;

	UFUNCTION(Category = "Rendering|Animation Trigger", BlueprintCallable)
	void	TriggerHit();
	
	UFUNCTION(NetMulticast, Unreliable, WithValidation)
	void	OnHitFX(FVector Location);

	void	RegisterAttacker(AArenaCharacter* Attacker);

protected:
	void	UpdateColor(const float DeltaTime);
	UFUNCTION(NetMulticast, Unreliable, WithValidation)
	void	SetColorStrength(const float value);

	UMaterialInstanceDynamic	*DynamicMat = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Damage Color", BlueprintReadOnly)
	FLinearColor				DefaultDamageColorRGB;

	UPROPERTY(Category = "Damage Color", BlueprintReadWrite, EditAnywhere)
	float	DamageLength = 1.0f;
	float	DamageTimer = 0.f;

	UPROPERTY(Category = "Rendering|Animation", Replicated, BlueprintReadWrite, VisibleAnywhere)
	bool	bIsAttacking = false;
	UPROPERTY(Category = "Rendering|Animation", Replicated, BlueprintReadWrite, VisibleAnywhere)
	bool	bIsAlive = true;

	AArenaCharacter		*LastAttacker = nullptr;

public:
#define FOOTPRINT_SURFACE_Sand     	EPhysicalSurface::SurfaceType1

	UFUNCTION(Category = "Rendering|Foots", BlueprintCallable)
	void	FootDown(const UArrowComponent* FootArrow) const;

	// Has to be set in Blueprint
	UPROPERTY(Category = "Rendering|Foots", EditAnywhere, BlueprintReadWrite)
	UArrowComponent	*FootLeft;

	// Has to be set in Blueprint
	UPROPERTY(Category = "Rendering|Foots", EditAnywhere, BlueprintReadWrite)
	UArrowComponent	*FootRight;

protected:
	UFUNCTION(Category = "Rendering|Foots")
	void	Trace(FHitResult& OutHit, const FVector& Location) const;
#pragma endregion


#pragma region	Target
public:
	UFUNCTION(BlueprintCallable, Category = Target)
	virtual void	SetTarget(AActor *Target);
	UFUNCTION(BlueprintCallable, Category = Target)
	virtual void	UnsetTarget();

protected:
	AActor	*TargetFocused = nullptr;	
#pragma endregion


#pragma region	Equipment
public:
	UFUNCTION()
	virtual void	OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(Category = "Equipment", BlueprintCallable)
	void	PickItem(AActor* OtherActor);
	UFUNCTION(Category = "Equipment", BlueprintCallable)
	bool	PickShield(APickableItemBase *pickedItem);
	UFUNCTION(Category = "Equipment", BlueprintCallable)
	bool	PickWeapon(APickableItemBase *pickedItem);
	bool	PickWeapon(APickableWeapon *pickedWeapon);

	void	TryThrowWeapon();
	void	TryThrowShield();

	bool	HasWeapon()	const;
	bool	HasShield()	const;
	
protected:
	UPROPERTY(Category = "Equipment", EditDefaultsOnly, BlueprintReadWrite)
	UClass	*DefaultShieldClass;

	UPROPERTY(Category = "Equipment", EditDefaultsOnly, BlueprintReadWrite)
	UClass	*DefaultWeaponClass;

	UPROPERTY(Category = "Equipment", EditAnywhere)
	float	LoseShieldProb = 0.3f;

	UPROPERTY(Category = "Equipment", EditAnywhere)
	float	LoseWeaponProb = 0.3f;

	UPROPERTY(Replicated, Category = "Equipment", VisibleAnywhere)
	APickableItemBase	*ShieldActor = nullptr;

	UPROPERTY(Replicated, Category = "Equipment", VisibleAnywhere)
	APickableWeapon		*WeaponActor = nullptr;
#pragma endregion


#pragma region RPC used for Networking
public:
	// Sets every UPROPERTY replicated
	virtual void	GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * Triggers a call to Server to tell Character is dead
	 * RPC: Server
	 * Should be overriden
	 */
	UFUNCTION(Server, Unreliable, WithValidation, Category = "RPC function|Server")
	virtual void	OnDeath();

	/**
	 * Triggers a call to Server to ask a particular Character to Attack()
	 * RPC: Server
	 */
	UFUNCTION(Server, Unreliable, WithValidation, Category = "RPC function|Server")
	virtual void	TriggerAttack();
	
protected:
	/**
	 * Notifies every client a particular Character is attacking
	 * RPC: NetMulticast
	 */
	UFUNCTION(NetMulticast, Unreliable, WithValidation, Category = "RPC function|NetMulticast")
	virtual void	Attack();
#pragma endregion

};
