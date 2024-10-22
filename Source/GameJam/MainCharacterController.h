// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "MainGameMode.h"
#include "MainCharacterController.generated.h"

UCLASS()
class GAMEJAM_API AMainCharacterController : public ACharacter
{
	GENERATED_BODY ()

public:
	//Sets default values for this character's properties
	AMainCharacterController ();

	//Called every frame
	virtual void Tick (float DeltaTime) override;

	//Called to bind functionality to input
	virtual void SetupPlayerInputComponent (class UInputComponent* PlayerInputComponent) override;

	void HitPolymorph ();

	UFUNCTION (BlueprintImplementableEvent, Category = "Character Controller")
	void OnHitBP ();

	UFUNCTION (BlueprintCallable)
	FVector ClientGetLaserTargetPosition ();

	UFUNCTION (BlueprintCallable)
	void GetStunned ();

	UFUNCTION (BlueprintCallable)
	void UpdateScore ();

	UPROPERTY (BlueprintReadOnly) FVector laserTargetPosition;
	UPROPERTY (Replicated, BlueprintReadOnly) float polymorphCharge = 0.0f;

	UPROPERTY (Replicated, BlueprintReadOnly) bool isPig = false;
	UPROPERTY (Replicated, BlueprintReadOnly) float pigTimer = 0.0f;

	UPROPERTY (Replicated, BlueprintReadOnly) bool showTargetPigTimer = false;
	UPROPERTY (Replicated, BlueprintReadOnly) float targetPigTimer = 0.0f;

	UPROPERTY (Replicated, BlueprintReadOnly) bool pigCharging = false;
	UPROPERTY (Replicated, BlueprintReadOnly) float pigChargeTimer = 0.0f;

	UPROPERTY (BlueprintReadOnly) FString gameTimerText = "";

	UPROPERTY (Replicated, BlueprintReadOnly) bool isStunned = false;

	UPROPERTY (BlueprintReadOnly) int yourScore = 0;
	UPROPERTY (BlueprintReadOnly) int firstEnemyScore = 0;
	UPROPERTY (BlueprintReadOnly) int secondEnemyScore = 0;

	UPROPERTY (BlueprintReadOnly) int firstEnemyIcon = 0;
	UPROPERTY (BlueprintReadOnly) int secondEnemyIcon = 0;

	UPROPERTY (Replicated, BlueprintReadOnly) int playerOneScore = 0;
	UPROPERTY (Replicated, BlueprintReadOnly) int playerTwoScore = 0;
	UPROPERTY (Replicated, BlueprintReadOnly) int playerThreeScore = 0;

	UPROPERTY (Replicated, BlueprintReadOnly) bool gameStarted = false;

	UPROPERTY (Replicated, BlueprintReadOnly) bool laserColor = false;

	UFUNCTION (BlueprintCallable)
	void SetPlayerIndex (int index);

	UFUNCTION (BlueprintCallable)
	void ResetPigChargeTimer ();

protected:
	//Called when the game starts or when spawned
	virtual void BeginPlay () override;

	UFUNCTION (BlueprintImplementableEvent, Category = "Character Controller")
	void ChangeMeshBP (int playerIndex);
	UFUNCTION (BlueprintImplementableEvent, Category = "Character Controller")
	void TurnIntoPigBP ();
	UFUNCTION (BlueprintImplementableEvent, Category = "Character Controller")
	void TurnIntoHumanBP ();
	UFUNCTION (BlueprintImplementableEvent, Category = "Character Controller")
	void StartPolymorphBP ();
	UFUNCTION (BlueprintImplementableEvent, Category = "Character Controller")
	void StopPolymorphBP ();
	UFUNCTION (BlueprintImplementableEvent, Category = "Character Controller")
	void ShootBP ();
	UFUNCTION (BlueprintImplementableEvent, Category = "Character Controller")
	void ChargeBP (FVector direction, float force);
	UFUNCTION (BlueprintImplementableEvent, Category = "Character Controller")
	void PlayeEmoteBP ();
	UFUNCTION (BlueprintImplementableEvent, Category = "Character Controller")
	void FinishGameBP ();

	UFUNCTION (BlueprintImplementableEvent, Category = "Character Controller")
	void ShootHitBP (FVector hitPosition);

private:
	UFUNCTION (Server, Reliable, WithValidation)
	void StartPolymorph ();
	UFUNCTION (Server, Reliable, WithValidation)
	void StopPolymorph ();

	void Polymorph ();

	UFUNCTION (Server, Reliable, WithValidation)
	void StartCharge ();
	UFUNCTION (Server, Reliable, WithValidation)
	void StopCharge (FVector direction);

	void Charge ();
	void StopChargeInput ();

	void TurnIntoPig ();
	void TurnIntoHuman ();

	void ShootInput ();
	void ClientShoot ();
	void ResetShootCooldown ();

	float _stunTimer = 0.0f;

	bool _hasPlayedEmote = false;

	bool _gameFinished = false;

	UFUNCTION (Server, Reliable, WithValidation)
	void Shoot (FVector startPosition, FVector endPosition);

	UFUNCTION (Server, Reliable, WithValidation)
	void SetShootingToFalse ();

	UFUNCTION (Server, Reliable, WithValidation)
	void ServerChangeMesh ();

	UPROPERTY (Replicated) bool _canShoot = true;
	bool _isShooting = false;

	UPROPERTY (Replicated) float _gameTimers = 600.0f;

	UPROPERTY (Replicated) bool _isPolymorphing = false;

	float _removeTargetPigTimerTimer = 0.0f;

	UPROPERTY (Replicated) int _playerIndex = 0;

	UCameraComponent* _cameraComponent;
	AMainGameMode* _gameMode;
};
