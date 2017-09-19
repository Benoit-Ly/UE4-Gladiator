// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"

// TODO: remove all these before release
#define LOG_TEMP(format, ...)	UE_LOG(LogTemp, Warning, format, __VA_ARGS__);
								//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::White, FString::Printf(format, __VA_ARGS__));

#define LOG_ERROR(format, ...)	UE_LOG(LogTemp, Error, format, __VA_ARGS__);
								//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Red, FString::Printf(format, __VA_ARGS__)); \