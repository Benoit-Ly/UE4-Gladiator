// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Helper.h"
#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "NetworkHelper.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_API UNetworkHelper : public UHelper
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Helpers", meta = (DisplayName = "Get Network Helper"))
	static UNetworkHelper* GetInstance();

	virtual void Init() override;

	UFUNCTION(BlueprintCallable, Category = "IP Address")
		FString GetIP();

private:
	static UNetworkHelper* Instance;
};
