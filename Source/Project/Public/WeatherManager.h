// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "WeatherManager.generated.h"

UENUM(BlueprintType)
enum class	EWeather : uint8
{
	Clear,
	Rain,
};

UCLASS()
class PROJECT_API AWeatherManager : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rain")
		TArray<UParticleSystem*>	ParticlesClasses;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rain")
	TArray<AActor *>	RainTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rain")
		AAtmosphericFog*	AtmosphereActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rain")
		float	RainingFogMultiplier = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rain")
		float	RainingFogDensity = 8.f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rain|Sand")
	AStaticMeshActor *WorldMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rain|Sand")
	int MaterialMeshIndex = 6;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rain|Sand")
	FLinearColor WetSandColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rain|Sand")
		float ChangeSandColorDelay = 2.0f;

	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rain")
	USoundBase * RainSound;
	
private:
	TArray<UParticleSystemComponent *>	RainParticles;
	void	SetRainParticles();

public:	
	// Sets default values for this actor's properties
	AWeatherManager();

	// Called when the game starts or when spawned
	virtual void	BeginPlay() override;
	
	// Called every frame
	virtual void	Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Weather")
		float	timeBeforeRain = 5.0f;

private:
	void	ClearWeather();
	bool	CheckForWeather(const float DeltaSeconds);

	void UpdateSandColor(const float DeltaTime);
	void UpdateFog(const float DeltaTime);

	EWeather	currWeather = EWeather::Clear;

	float ChangeSandColorTimer = 0.f;

	UMaterialInstanceDynamic *DynamicMat = nullptr;
	UAudioComponent * AudioComponent = nullptr;


#pragma region RPC used for Networking

public:

	/*
	* Changes weather to something in EWeather
	* RPC: Server
	*/
	UFUNCTION(Server, Reliable, WithValidation)
		void	ChangeWeather(const EWeather weather);

	UPROPERTY(BlueprintReadOnly, Category = "DayNight")
	bool bEnableDayNightCycle = false;

	/*
	* Notifies every client to start tthe DayNight Cycle
	* RPC: NetMulticast
	*/
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void	StartDayNightCycle();

	/*
	* Notifies every client to stop tthe DayNight Cycle
	* RPC: NetMulticast
	*/
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void	StopDayNightCycle();


	/*
	* Notifies every client it's starting to rain
	* RPC: NetMulticast
	*/
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void	MakeItRain();

	/*
	* Notifies every client it's not raining anymore
	* RPC: NetMulticast
	*/
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void	StopRain();
#pragma endregion




	bool IsRaining();

#define FOOTPRINT_SURFACE_Default   EPhysicalSurface::SurfaceType_Default
#define FOOTPRINT_SURFACE_Sand     	EPhysicalSurface::SurfaceType1
#define FOOTPRINT_SURFACE_WetSand   EPhysicalSurface::SurfaceType2
};
