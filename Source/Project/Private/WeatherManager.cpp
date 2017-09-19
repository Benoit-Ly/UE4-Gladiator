// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "WeatherManager.h"
#include "UnrealNetwork.h"

// Sets default values
AWeatherManager::AWeatherManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;

	AudioComponent = CreateDefaultSubobject<UAudioComponent>("AudioComp");
	AudioComponent->bAutoActivate = false;
	AudioComponent->bAllowSpatialization = true;

	RootComponent = AudioComponent;

}

// Called when the game starts or when spawned
void	AWeatherManager::BeginPlay()
{
	Super::BeginPlay();

	AudioComponent->SetSound(RainSound);

	timeBeforeRain = (10 + rand() % (20)) + 1;

	ChangeSandColorTimer = ChangeSandColorDelay;

	
	SetRainParticles();

}

void	AWeatherManager::SetRainParticles()
{
	UWorld *world = GetWorld();

	if (world == NULL)
		return;

	for (AActor *oneTarget : RainTarget)
	{
		for (UParticleSystem *	Particle : ParticlesClasses)
		{
			UParticleSystemComponent* ParticleTmp;
			ParticleTmp = UGameplayStatics::SpawnEmitterAtLocation(this, Particle,
				oneTarget->GetActorLocation(), oneTarget->GetActorRotation(), false);
			ParticleTmp->Deactivate();
			RainParticles.Add(ParticleTmp);
		}

	}

	if (!WorldMesh)
		return;

	UMaterialInterface* Material = WorldMesh->GetStaticMeshComponent()->GetMaterial(MaterialMeshIndex);
	DynamicMat = UMaterialInstanceDynamic::Create(Material, this);

	WorldMesh->GetStaticMeshComponent()->SetMaterial(MaterialMeshIndex, DynamicMat);
	DynamicMat->GetPhysicalMaterial()->SurfaceType = FOOTPRINT_SURFACE_Sand;

}

// Called every frame
void	AWeatherManager::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	ChangeSandColorTimer = FMath::Clamp(ChangeSandColorTimer + DeltaTime, 0.f, ChangeSandColorDelay);

	UpdateSandColor(DeltaTime);
	UpdateFog(DeltaTime);

	if (HasAuthority() == false)
		return;

	if (CheckForWeather(DeltaTime))
	{
		switch (currWeather)
		{
		case	EWeather::Rain:
			ChangeWeather(EWeather::Clear);
			break;
		case	EWeather::Clear:
			ChangeWeather(EWeather::Rain);
			break;
		}
	}
}

bool	AWeatherManager::CheckForWeather(const float DeltaTime)
{
	bool	needToChangeWeather = false;
	timeBeforeRain -= DeltaTime;

	if (timeBeforeRain <= 0)
	{
		needToChangeWeather = true;
		timeBeforeRain = (10 + rand() % (20)) + 1;
	}

	return needToChangeWeather;
}

void	AWeatherManager::UpdateFog(const float DeltaTime)
{
	if (!AtmosphereActor)
		return;

	if (ChangeSandColorTimer == ChangeSandColorDelay)
		return;

	float factor = ChangeSandColorTimer / ChangeSandColorDelay;

	float NewFogMultiplier, NewFogDensity;

	switch (currWeather)
	{
	case EWeather::Rain:
		NewFogMultiplier = FMath::Lerp(1.f, RainingFogMultiplier, factor);
		NewFogDensity = FMath::Lerp(1.f, RainingFogDensity, factor);
		break;
	default:
		NewFogMultiplier = FMath::Lerp(RainingFogMultiplier, 1.f, factor);
		NewFogDensity = FMath::Lerp(RainingFogDensity, 1.f, factor);
		break;
	}
	AtmosphereActor->GetAtmosphericFogComponent()->SetFogMultiplier(NewFogMultiplier);
	AtmosphereActor->GetAtmosphericFogComponent()->SetDensityMultiplier(NewFogDensity);
}

void	AWeatherManager::UpdateSandColor(const float DeltaTime)
{
	if (!DynamicMat)
		return;

	if (ChangeSandColorTimer == ChangeSandColorDelay)
		return;

	FVector NewColorVector;

	float factor = ChangeSandColorTimer / ChangeSandColorDelay;

	float X = factor * (1.f - WetSandColor.R);
	float Y = factor * (1.f - WetSandColor.G);
	float Z = factor * (1.f - WetSandColor.B);

	switch (currWeather)
	{
	case EWeather::Rain:
		NewColorVector = FVector(1.f, 1.f, 1.f) - FVector(X, Y, Z);
		break;
	default:
		NewColorVector = FVector(X, Y, Z) + FVector(WetSandColor.R, WetSandColor.G, WetSandColor.B);
		break;
	}
	DynamicMat->SetVectorParameterValue("FilterSand", FLinearColor(NewColorVector.X,
		NewColorVector.Y,
		NewColorVector.Z, 1));
}

void	AWeatherManager::ClearWeather()
{
	switch (currWeather)
	{
	case EWeather::Rain:
		StopRain();
		break;
	}

	currWeather = EWeather::Clear;
}

bool AWeatherManager::IsRaining() {
	return currWeather == EWeather::Rain;
}

#pragma region RPC used for Networking

/*
** Function used when calling ChangeWeather() - see .h
*/
void	AWeatherManager::ChangeWeather_Implementation(const EWeather weather)
{
	switch (weather)
	{
	case EWeather::Rain:
		MakeItRain();
		break;
	default:
		ClearWeather();
		break;
	}
}

/*
** Function checking if allowed to call ChangeWeather()
*/
bool	AWeatherManager::ChangeWeather_Validate(const EWeather weather)
{
	return true;
}

/*
** Function used when calling MakeItRain() - see .h
*/
void	AWeatherManager::MakeItRain_Implementation()
{
	for (UParticleSystemComponent *oneRainParticle : RainParticles)
	{
		oneRainParticle->Activate();
	}

	currWeather = EWeather::Rain;
	ChangeSandColorTimer = 0.f;

	if (RainSound)
		AudioComponent->FadeIn(ChangeSandColorDelay, 1.0f, 0);

	if (DynamicMat)
		DynamicMat->GetPhysicalMaterial()->SurfaceType = FOOTPRINT_SURFACE_WetSand;
}	

/*
** Function checking if allowed to call MakeItRain()
*/
bool	AWeatherManager::MakeItRain_Validate()
{
	return true;
}

/*
** Function used when calling StopRain() - see .h
*/
void	AWeatherManager::StopRain_Implementation()
{
	for (UParticleSystemComponent *oneRainParticle : RainParticles)
	{
		oneRainParticle->Deactivate();
	}

	ChangeSandColorTimer = 0.f;
	currWeather = EWeather::Clear;

	if (RainSound)
		AudioComponent->FadeOut(ChangeSandColorDelay, 0);
	

	if (DynamicMat)
		DynamicMat->GetPhysicalMaterial()->SurfaceType = FOOTPRINT_SURFACE_Sand;
}

/*
** Function checking if allowed to call StopRain()
*/
bool	AWeatherManager::StopRain_Validate()
{
	return true;
}

bool	AWeatherManager::StartDayNightCycle_Validate()
{
	return true;
}

void	AWeatherManager::StartDayNightCycle_Implementation()
{
	bEnableDayNightCycle = true;
}

bool	AWeatherManager::StopDayNightCycle_Validate()
{
	return true;
}

void	AWeatherManager::StopDayNightCycle_Implementation()
{
	bEnableDayNightCycle = false;
}

#pragma endregion