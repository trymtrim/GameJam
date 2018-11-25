// Fill out your copyright notice in the Description page of Project Settings.

#include "MainGameMode.h"
#include "GameFramework/Character.h"
#include "ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "MainCharacterController.h"

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

	if (gameStarted && !gameFinished)
		gameTimer -= DeltaTime;

	if (gameTimer <= 0.0f && !gameFinished)
		FinishGame ();
}

void AMainGameMode::FinishGame ()
{
	gameTimer = 0.0f;
	gameStarted = false;
	gameFinished = true;
}

void AMainGameMode::UpdatePlayerScore (int playerIndex)
{
	switch (playerIndex)
	{
	case 0:
		playerOneScore++;
		break;
	case 1:
		playerTwoScore++;
		break;
	case 2:
		playerThreeScore++;
		break;
	}

	for (int i = 0; i < _characters.Num (); i++)
	{
		_characters [i]->playerOneScore = playerOneScore;
		_characters [i]->playerTwoScore = playerTwoScore;
		_characters [i]->playerThreeScore = playerThreeScore;
	}
}

void AMainGameMode::RegisterPlayer (AMainCharacterController* characterController)
{
	_characters.Add (characterController);
}

int AMainGameMode::GetPlayerIndex ()
{
	_playerCharacters++;

	if (_playerCharacters >= 3)
		gameStarted = true;

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
