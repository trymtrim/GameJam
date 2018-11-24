// Fill out your copyright notice in the Description page of Project Settings.

#include "MainGameMode.h"
#include "GameFramework/Character.h"
#include "ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"

AMainGameMode::AMainGameMode ()
{
	//Set default pawn class
	static ConstructorHelpers::FClassFinder <APawn> PlayerPawnClass (TEXT ("/Game/Blueprints/MainCharacterControllerBP"));

	if (PlayerPawnClass.Class != NULL)
		DefaultPawnClass = PlayerPawnClass.Class;

	//Set default player controller class
	//PlayerControllerClass = AMainPlayerController::StaticClass ();

	//Set default game state class
	//GameStateClass = AMainGameState::StaticClass ();
}

//Called every frame
void AMainGameMode::Tick (float DeltaTime)
{
	Super::Tick (DeltaTime);

	gameTimer -= DeltaTime;

	if (gameTimer <= 0.0f && !gameFinished)
		FinishGame ();
}

void AMainGameMode::FinishGame ()
{
	gameFinished = true;
}

int AMainGameMode::GetPlayerIndex ()
{
	_playerCharacters++;

	return _playerCharacters - 1;
}

AActor* AMainGameMode::ChoosePlayerStart_Implementation (AController* Player)
{
	//TODO, fix this and do stuff properly

	TArray <AActor*> playerStarts;
	UGameplayStatics::GetAllActorsOfClass (GetWorld (), APlayerStart::StaticClass (), playerStarts);

	if (_playerCount == _maxPlayers)
	{
		if (_currentPlayerStartIndex == playerStarts.Num () - 1)
			_currentPlayerStartIndex = 0;
		else
			_currentPlayerStartIndex++;

		return playerStarts [_currentPlayerStartIndex];
	}

	if (_playerCount < _maxPlayers)
		_playerCount++;

	return playerStarts [_playerCount - 1];
}
