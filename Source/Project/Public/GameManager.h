// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <memory>
#include "Engine/GameInstance.h"
#include "Manager.h"
#include "Helper.h"
#include "GameManager.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_API UGameManager : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = Managers, meta = (DisplayName = "Get Game Manager"))
	static UGameManager* GetInstance();

	virtual void Init() override;

	void CreateManagerFromClass(UClass* ManagerClass);
	void CreateHelperFromClass(UClass* HelperClass);

	UFUNCTION(BlueprintCallable, Category = Managers)
	void RegisterManager(UManager* newManager);

	void RegisterHelper(UHelper* newHelper);

	virtual void Shutdown() override;

	void ExitGame();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Managers)
		TArray<TSubclassOf<UManager>> ManagerList;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Helpers)
		TArray<TSubclassOf<UHelper>> HelperList;

private:
	static UGameManager* Instance;

	TArray<UManager*> ManagerInstances;
	TArray<UHelper*> HelperInstances;
};
