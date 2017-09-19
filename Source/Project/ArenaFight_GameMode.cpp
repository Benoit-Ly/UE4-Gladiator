// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "ArenaFight_GameMode.h"
#include "ArenaFight_GameState.h"
#include "AIDirector.h"
#include "ArenaCharacter.h"
#include "PlayerCharacter.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h" 

AArenaFight_GameMode::AArenaFight_GameMode()
{
	bUseSeamlessTravel = true;
	bDelayedStart = true;
}

#pragma region	Overriden from AGameMode

void	AArenaFight_GameMode::BeginPlay()
{
	Super::BeginPlay();
	CustomBeginPlay();
}

#pragma region Seamless Travel Handling
void	AArenaFight_GameMode::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();
	CustomBeginPlay(); // To avoid bug: Report UE-27284

	LOG_TEMP(TEXT("PostSeamlessTravel() - PLAYERS WANTED [%d] | PLAYERS [%d] | CONTROLLERS [%d] | PLAYERS TRAVELLING [%d]"),
		PlayersWanted, NumPlayers, listPlayers.Num(), NumTravellingPlayers);

	if (HasEnoughPlayers()) // Avoid bug where called after WaitingToStart
		SetupAndStartGame();
}

void	AArenaFight_GameMode::HandleSeamlessTravelPlayer(AController *&oldController)
{
	Super::HandleSeamlessTravelPlayer(oldController);
	CustomBeginPlay(); // To avoid bug: Report UE-27284

	LOG_TEMP(TEXT("HandleSeamlessTravelPlayer() - PLAYERS WANTED [%d] | PLAYERS [%d] | CONTROLLERS [%d] | PLAYERS TRAVELLING [%d]"),
		PlayersWanted, NumPlayers, listPlayers.Num(), NumTravellingPlayers);

	APlayerController	*oldPlayer = Cast<APlayerController>(oldController);

	listPlayers.AddUnique(oldPlayer);

	if (HasEnoughPlayers()) // Avoid bug where called after WaitingToStart
		SetupAndStartGame();
}
#pragma endregion

void	AArenaFight_GameMode::PostLogin(APlayerController *newPlayer)
{
	Super::PostLogin(newPlayer);
	CustomBeginPlay(); // To avoid bug: Report UE-27284

	LOG_TEMP(TEXT("AArenaFight_GameMode::PostLogin() - ENTER || Wanted Players == [%d]"), PlayersWanted);

	if (HasEnoughPlayers() == true)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Already have enough players! - Don't accept request.")));
		LOG_TEMP(TEXT("AArenaFight_GameMode::PostLogin() - Already have enough players! - Don't accept request."));
		return;
	}

	listPlayers.Add(newPlayer);

	if (HasEnoughPlayers() == false)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("New player has been connected! - Waiting for more.")));
		LOG_TEMP(TEXT("AArenaFight_GameMode::PostLogin() - New player has been connected! - Waiting for more."));
		return;
	}

	SetupAndStartGame();
}

void	AArenaFight_GameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();

	if (HasEnoughPlayers())
		SetupAndStartGame();
}

void	AArenaFight_GameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Let's go!")));
	LOG_TEMP(TEXT("AArenaFight_GameMode::HandleMatchHasStarted() - Let's go!"));

	ClientsUseMatinee(false);
	SetPauseAllControllers(false);
}

void	AArenaFight_GameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	FName	currentMatchState = GetMatchState();

	if (currentMatchState == MatchState::WaitingToStart)
	{
		// Managed in HandleMatchIsWaitingToStart()
	}
	else if (currentMatchState == MatchState::ReadyToStart)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Get ready!")));
		LOG_TEMP(TEXT("AArenaFight_GameMode::OnMatchStateSet() - Get ready!"));

		SetPauseAllControllers(true);
		ClientsUseMatinee(true);

		// Delay CustomStartMatch() call
		FLatentActionInfo latentActionInfo;
		latentActionInfo.CallbackTarget = this;
		latentActionInfo.ExecutionFunction = "CustomStartMatch";
		latentActionInfo.Linkage = 1;
		UKismetSystemLibrary::Delay(this, 5.0f, latentActionInfo);
	}
	else if (currentMatchState == MatchState::InProgress)
	{
		// Managed in HandleMatchHasStarted()
	}
}

#pragma endregion

void	AArenaFight_GameMode::CustomBeginPlay()
{
	if (bIsInit)
		return;

	ParseLaunchOptions();

	LOG_TEMP(TEXT("AArenaFight_GameMode::CustomBeginPlay() - Wanted Players == [%d]"), PlayersWanted);

	{
		TArray<AActor *> foundDirectors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAIDirector::StaticClass(), foundDirectors);

		if (foundDirectors.Num() > 0)
			aiDirector = Cast<AAIDirector>(foundDirectors[0]);
	}

	bIsInit = true;
}

void	AArenaFight_GameMode::ParseLaunchOptions()
{
	PlayersWanted = FCString::Atoi(*(UGameplayStatics::ParseOption(OptionsString, "NumPlayersWanted")));
	if (PlayersWanted <= 0)
		PlayersWanted = 1;
}

void	AArenaFight_GameMode::SetupAndStartGame()
{
	if (GetMatchState() != MatchState::WaitingToStart)
		return;

	StartPlay();

	SetupPlayers();

	StartNextWave();

	SetMatchState(MatchState::ReadyToStart);
}

void	AArenaFight_GameMode::SetupPlayers()
{
	UWorld	*world = GetWorld();
	if (!world)
		return;

	for (APlayerController *oneController : listPlayers)
	{
		AActor	*playerStart = FindPlayerStart(oneController);
		if (!playerStart)
			continue;

		AArenaCharacter	*oneCharacter = world->SpawnActor<AArenaCharacter>(DefaultPawnClass, playerStart->GetActorTransform());
		if (!oneCharacter)
		{
			LOG_TEMP(TEXT("AArenaFight_GameMode::SetupPlayers() - couldn't cast to ArenaCharacter."));
			continue;
		}
		oneCharacter->SetOwner(oneController);
		oneController->Possess(oneCharacter);
		oneCharacter->InstantiateDefaultItems();
	}
}

void	AArenaFight_GameMode::CustomStartMatch()
{
	SetMatchState(MatchState::InProgress);
}

void	AArenaFight_GameMode::StartNextWave()
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Starting new wave!")));
	LOG_TEMP(TEXT("AArenaFight_GameMode::StartNextWave() - Starting new wave!"));

	AArenaFight_GameState	*currentGameState = (GetWorld() != NULL ? GetWorld()->GetGameState<AArenaFight_GameState>() : NULL);

	if (aiDirector && currentGameState)
		aiDirector->SpawnAIs(currentGameState->GetNextWave(), NumPlayers);
}

bool	AArenaFight_GameMode::HasEnoughPlayers() const
{
	return PlayersWanted == listPlayers.Num();
}

void	AArenaFight_GameMode::SetPauseAllControllers(const bool bPause)
{
	for (APlayerController	*onePlayerController : listPlayers)
	{
		APlayerCharacter	*onePlayerCharacter = Cast<APlayerCharacter>(onePlayerController->GetCharacter());

		if (onePlayerCharacter)
		{
			if (bPause)
			{
				onePlayerCharacter->CustomDisableInput();
			}
			else
			{
				onePlayerCharacter->CustomEnableInput();
			}
		}
		else
			LOG_TEMP(TEXT("AArenaFight_GameMode::SetPauseAllControllers() - couldn't get PlayerCharacter from Controller"));
	}

	if (aiDirector)
		aiDirector->AIsSetEnabled(!bPause);
}

void	AArenaFight_GameMode::ClientsUseMatinee(const bool bEnable)
{
	for (APlayerController *onePlayerController : listPlayers)
	{
		APlayerCharacter	*onePlayerCharacter = Cast<APlayerCharacter>(onePlayerController->GetCharacter());

		if (onePlayerCharacter)
		{
			onePlayerCharacter->UseMatinee(bEnable);
		}
	}
}