// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MainGameMode.generated.h"

UCLASS()
class GAMEJAM_API AMainGameMode : public AGameMode
{
	GENERATED_BODY ()
	
public:
	AMainGameMode ();

	//Called every frame
	virtual void Tick (float DeltaTime) override;

	virtual AActor* ChoosePlayerStart_Implementation (AController* Player) override;

	int GetPlayerIndex ();

	float gameTimer = 600.0f;
	bool gameFinished = false;

private:
	void FinishGame ();

	int _maxPlayers = 3;
	int _playerCount = 0;
	int _currentPlayerStartIndex = 0;

	int _playerCharacters = 0;
};
