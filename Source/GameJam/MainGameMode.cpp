// Fill out your copyright notice in the Description page of Project Settings.

#include "MainGameMode.h"
#include "GameFramework/Character.h"
#include "ConstructorHelpers.h"

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
