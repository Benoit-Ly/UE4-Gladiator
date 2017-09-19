// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "Manager.generated.h"

/**
 * 
 */
UCLASS(abstract)
class PROJECT_API UManager : public UObject
{
	GENERATED_BODY()

public:
	virtual void Init();
	
	
};
