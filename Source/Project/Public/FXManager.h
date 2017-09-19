// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Manager.h"
#include "FXManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAction);

UCLASS(Blueprintable)
class PROJECT_API UFXManager : public UManager
{
	GENERATED_BODY()
	
public:	

	UFUNCTION(BlueprintCallable, Category = "Managers", meta = (DisplayName = "Get FX Manager"))
	static UFXManager* GetInstance();

	virtual void Init() override;

	UParticleSystem* GetDamageEffect();

	UParticleSystem* GetFootstepSandEffect();

	USoundBase* GetDamageSound();

	UClass* GetFootstepSandDecal();


	void CallOnItemFar();
	void CallOnItemNear();

	void CallOnLockTarget();
	void CallOnUnlockTarget();
	

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UParticleSystem* DamageEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundBase* DamageSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UParticleSystem* FootstepSandEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UClass* FootstepSandDecal;

	UPROPERTY(BlueprintAssignable)
		FOnAction	OnItemFar;
	UPROPERTY(BlueprintAssignable)
		FOnAction	OnItemNear;
	UPROPERTY(BlueprintAssignable)
		FOnAction	OnLockTarget;
	UPROPERTY(BlueprintAssignable)
		FOnAction	OnUnlockTarget;

private:
	static UFXManager* Instance;
};
