// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MainGameMode.generated.h"

class AMainCharacterController;

UCLASS()
class GAMEJAM_API AMainGameMode : public AGameMode
{
	GENERATED_BODY ()
	
public:
	AMainGameMode ();

	//Called every frame
	virtual void Tick (float DeltaTime) override;

	virtual AActor* ChoosePlayerStart_Implementation (AController* Player) override;

	UFUNCTION (BlueprintCallable)
	void UpdatePlayerScore (int playerIndex);

	void RegisterPlayer (AMainCharacterController* characterController);

	int GetPlayerIndex ();

	float gameTimer = 10.0f;
	bool gameFinished = false;

	int playerOneScore = 0;
	int playerTwoScore = 0;
	int playerThreeScore = 0;

	bool gameStarted = false;

private:
	void FinishGame ();

	TArray <AMainCharacterController*> _characters;

	int _maxPlayers = 3;
	int _playerCount = 0;
	int _currentPlayerStartIndex = 0;

	int _playerCharacters = 0;
};
