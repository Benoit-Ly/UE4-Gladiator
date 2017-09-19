// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Manager.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "GUIManager.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMessageDisappear, UUserWidget*, Widget);

UCLASS(Blueprintable)
class PROJECT_API UGUIManager : public UManager
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Managers", meta = (DisplayName = "Get GUI Manager"))
	static UGUIManager* GetInstance();

	virtual void Init() override;

	UFUNCTION(BlueprintCallable, Category = "GUI")
	void ReportDisappear(UUserWidget* widget);

	UPROPERTY(BlueprintAssignable)
		FOnMessageDisappear OnMessageDisappear;

private:
	static UGUIManager* Instance;
	
	
};
