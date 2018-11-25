// Fill out your copyright notice in the Description page of Project Settings.

#include "MainCharacterController.h"
#include "UnrealNetwork.h"
#include "Engine/World.h"

//Sets default values
AMainCharacterController::AMainCharacterController ()
{
 	//Set this character to call Tick () every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

//Called when the game starts or when spawned
void AMainCharacterController::BeginPlay ()
{
	Super::BeginPlay ();

	//Get camera component
	TArray <UCameraComponent*> cameraComps;
	GetComponents <UCameraComponent> (cameraComps);
	_cameraComponent = cameraComps [0];

	if (GetWorld ()->IsServer ())
	{
		_gameMode = Cast <AMainGameMode> (GetWorld ()->GetAuthGameMode ());
		_gameMode->RegisterPlayer (this);
	}
	else
		ServerChangeMesh ();
}

//Called every frame
void AMainCharacterController::Tick (float DeltaTime)
{
	Super::Tick (DeltaTime);

	if (GetWorld ()->IsServer ())
	{
		if (_isPolymorphing)
			Polymorph ();

		if (polymorphCharge > 0.0f)
		{
			polymorphCharge -= DeltaTime / 10;

			if (polymorphCharge < 0.0f)
				polymorphCharge = 0.0f;
		}

		if (isPig)
		{
			pigTimer -= DeltaTime;

			if (pigTimer <= 0.0f)
				TurnIntoHuman ();
		}

		if (_removeTargetPigTimerTimer > 0.0f)
		{
			_removeTargetPigTimerTimer -= DeltaTime;

			if (_removeTargetPigTimerTimer <= 0.0f)
				showTargetPigTimer = false;
		}

		_gameTimers = _gameMode->gameTimer;

		if (pigCharging)
			Charge ();

		if (isStunned)
		{
			_stunTimer += DeltaTime;

			if (_stunTimer >= 5.0f || isPig)
			{
				isStunned = false;
				_stunTimer = 0.0f;
			}
		}

		if (isPig)
		{
			if (pigChargeTimer > 0.0f)
			{
				pigChargeTimer -= 0.15f * DeltaTime;

				if (pigChargeTimer < 0.0f)
					pigChargeTimer = 0.0f;
			}
		}

		if (!gameStarted)
		{
			if (_gameMode->gameStarted)
				gameStarted = true;
		}
		else if (!_gameFinished)
		{
			if (_gameMode->gameFinished)
			{
				gameStarted = false;
				_gameFinished = true;

				FinishGameBP ();
			}
		}
	}
	else
	{
		int minutes = FMath::FloorToInt (_gameTimers / 60);
		int seconds = FMath::RoundToInt ((int) _gameTimers % 60);

		FString minutesString = "";
		FString secondsString = "";

		if (minutes < 10)
			minutesString = "0" + FString::FromInt (minutes);
		else
			minutesString = FString::FromInt (minutes);

		if (seconds < 10)
			secondsString = "0" + FString::FromInt (seconds);
		else
			secondsString = FString::FromInt (seconds);

		gameTimerText = minutesString + ":" + secondsString;

		if (_isShooting)
		{
			if (_isPolymorphing || isStunned)
				_isShooting = false;
			else
				ClientShoot ();
		}

		if (_playerIndex == 0)
		{
			yourScore = playerOneScore;

			firstEnemyScore = playerTwoScore;
			secondEnemyScore = playerThreeScore;

			firstEnemyIcon = 1;
			secondEnemyIcon = 2;
		}
		else if (_playerIndex == 1)
		{
			yourScore = playerTwoScore;

			firstEnemyScore = playerThreeScore;
			secondEnemyScore = playerOneScore;

			firstEnemyIcon = 2;
			secondEnemyIcon = 0;
		}
		else if (_playerIndex == 2)
		{
			yourScore = playerThreeScore;

			firstEnemyScore = playerOneScore;
			secondEnemyScore = playerTwoScore;

			firstEnemyIcon = 0;
			secondEnemyIcon = 1;
		}

		if (_gameTimers <= 90.0f && !_hasPlayedEmote && yourScore > firstEnemyScore && yourScore > secondEnemyScore)
		{
			_hasPlayedEmote = true;
			PlayeEmoteBP ();
		}
	}
}

void AMainCharacterController::ResetPigChargeTimer ()
{
	pigChargeTimer = 0.0f;
}

void AMainCharacterController::SetPlayerIndex (int index)
{
	_playerIndex = index;
}

void AMainCharacterController::UpdateScore ()
{
	_gameMode->UpdatePlayerScore (_playerIndex);
}

void AMainCharacterController::ServerChangeMesh_Implementation ()
{
	ChangeMeshBP (_gameMode->GetPlayerIndex ());
}

bool AMainCharacterController::ServerChangeMesh_Validate ()
{
	return true;
}

void AMainCharacterController::StartPolymorph_Implementation ()
{
	if (isPig || _isShooting || isStunned || !gameStarted)
		return;

	_isPolymorphing = true;
	StartPolymorphBP ();
}

bool AMainCharacterController::StartPolymorph_Validate ()
{
	return true;
}

void AMainCharacterController::StopPolymorph_Implementation ()
{
	if (isPig || !gameStarted)
		return;

	_isPolymorphing = false;
	StopPolymorphBP ();

	laserColor = false;
}

bool AMainCharacterController::StopPolymorph_Validate ()
{
	return true;
}

void AMainCharacterController::Polymorph ()
{
	if (isPig || isStunned)
	{
		StopPolymorph ();
		return;
	}

	//Line trace from camera to check if there is something in the crosshair's sight
	FCollisionQueryParams traceParams = FCollisionQueryParams (FName (TEXT ("RV_Trace")), true, this);
	traceParams.bTraceComplex = true;
	traceParams.bReturnPhysicalMaterial = false;

	FHitResult hit (ForceInit);

	//Declare start and end position of the line trace based on camera position and rotation
	FVector start = _cameraComponent->GetComponentLocation ();
	FVector end = _cameraComponent->GetComponentLocation () + (_cameraComponent->GetForwardVector () * 100000.0f);

	//Check if line trace hits anything
	if (GetWorld ()->LineTraceSingleByChannel (hit, start, end, ECC_Visibility, traceParams))
	{
		laserTargetPosition = hit.Location;

		if (hit.GetActor () == nullptr)
		{
			laserColor = false;
			return;
		}

		//If line trace hits a player, polymorph the target
		if (hit.GetActor ()->ActorHasTag ("Player"))
		{
			AMainCharacterController* player = Cast <AMainCharacterController> (hit.GetActor ());
			
			if (!player->isPig)
			{
				player->HitPolymorph ();

				showTargetPigTimer = true;
				targetPigTimer = player->polymorphCharge;

				_removeTargetPigTimerTimer = 1.0f;

				laserColor = true;
			}
			else
				laserColor = false;
		}
		else
			laserColor = false;
	}
	else
	{
		laserTargetPosition = _cameraComponent->GetComponentLocation () + (_cameraComponent->GetForwardVector () * 5000.0f);
		laserColor = false;
	}
}

void AMainCharacterController::ShootInput ()
{
	if (!gameStarted)
		return;

	if (isPig)
	{
		StartCharge ();
		return;
	}
	else if (!isStunned)
		_isShooting = true;
}

void AMainCharacterController::ClientShoot ()
{
	if (isStunned)
		_isShooting = false;

	//Declare start and end position of the line trace based on camera position and rotation
	FVector start = _cameraComponent->GetComponentLocation ();
	FVector end = _cameraComponent->GetComponentLocation () + (_cameraComponent->GetForwardVector () * 100000.0f);

	Shoot (start, end);
}

void AMainCharacterController::Shoot_Implementation (FVector startPosition, FVector endPosition)
{
	if (isPig || _isPolymorphing || !_canShoot || isStunned)
		return;

	_isShooting = true;

	ShootBP ();

	//Shoot cooldown
	_canShoot = false;
	FTimerHandle shootCooldownTimerHandle;
	GetWorld ()->GetTimerManager ().SetTimer (shootCooldownTimerHandle, this, &AMainCharacterController::ResetShootCooldown, 0.45f, false);

	//Line trace from camera to check if there is something in the crosshair's sight
	FCollisionQueryParams traceParams = FCollisionQueryParams (FName (TEXT ("RV_Trace")), true, this);
	traceParams.bTraceComplex = true;
	traceParams.bReturnPhysicalMaterial = false;

	FHitResult hit (ForceInit);

	//Declare start and end position of the line trace based on camera position and rotation
	FVector start = startPosition;
	FVector end = endPosition;

	//Check if line trace hits anything
	if (GetWorld ()->LineTraceSingleByChannel (hit, start, end, ECC_Visibility, traceParams))
	{
		ShootHitBP (hit.Location);

		if (hit.GetActor () == nullptr)
			return;

		//GEngine->AddOnScreenDebugMessage (-1, 15.0f, FColor::Yellow, hit.GetActor ()->GetName ());

		//If line trace hits a player, polymorph the target
		if (hit.GetActor ()->ActorHasTag ("Player"))
		{
			AMainCharacterController* player = Cast <AMainCharacterController> (hit.GetActor ());

			if (player->isPig)
				player->OnHitBP ();
		}
	}
}

bool AMainCharacterController::Shoot_Validate (FVector startPosition, FVector endPosition)
{
	return true;
}

void AMainCharacterController::ResetShootCooldown ()
{
	_canShoot = true;
}

void AMainCharacterController::StartCharge_Implementation ()
{
	if (pigChargeTimer > 0.0f)
		return;

	pigCharging = true;
}

bool AMainCharacterController::StartCharge_Validate ()
{
	return true;
}

void AMainCharacterController::StopCharge_Implementation (FVector direction)
{
	if (!pigCharging)
		return;

	//Do the charge
	if (isPig)
	{
		if (pigChargeTimer < 0.4f)
			pigChargeTimer = 0.4f;

		ChargeBP (direction, pigChargeTimer);
	}

	pigCharging = false;
}

bool AMainCharacterController::StopCharge_Validate (FVector direction)
{
	return true;
}

void AMainCharacterController::Charge ()
{
	pigChargeTimer += GetWorld ()->DeltaTimeSeconds;

	if (pigChargeTimer > 1.0f)
		pigChargeTimer = 1.0f;

	if (!isPig)
		pigCharging = false;
}

void AMainCharacterController::StopChargeInput ()
{
	if (!gameStarted)
		return;

	if (isPig)
	{
		FVector direction;

		//Line trace from camera to check if there is something in the crosshair's sight
		FCollisionQueryParams traceParams = FCollisionQueryParams (FName (TEXT ("RV_Trace")), true, this);
		traceParams.bTraceComplex = true;
		traceParams.bReturnPhysicalMaterial = false;

		FHitResult hit (ForceInit);

		//Declare start and end position of the line trace based on camera position and rotation
		FVector start = _cameraComponent->GetComponentLocation ();
		FVector end = _cameraComponent->GetComponentLocation () + (_cameraComponent->GetForwardVector () * 10000.0f);

		//Check if line trace hits anything
		if (GetWorld ()->LineTraceSingleByChannel (hit, start, end, ECC_Visibility, traceParams))
			direction = hit.ImpactPoint - GetActorLocation ();
		else //If line trace doesn't hit anything, spawn bullet with rotation towards the end of the line trace
			direction = end - GetActorLocation ();

		direction.Normalize ();

		//_cameraComponent->GetForwardVector ()
		StopCharge (direction);
	}

	if (_isShooting)
		_isShooting = false;

	SetShootingToFalse ();
}

void AMainCharacterController::GetStunned ()
{
	isStunned = true;
}

void AMainCharacterController::SetShootingToFalse_Implementation ()
{
	_isShooting = false;
}

bool AMainCharacterController::SetShootingToFalse_Validate ()
{
	return true;
}


FVector AMainCharacterController::ClientGetLaserTargetPosition ()
{
	//Line trace from camera to check if there is something in the crosshair's sight
	FCollisionQueryParams traceParams = FCollisionQueryParams (FName (TEXT ("RV_Trace")), true, this);
	traceParams.bTraceComplex = true;
	traceParams.bReturnPhysicalMaterial = false;

	FHitResult hit (ForceInit);

	//Declare start and end position of the line trace based on camera position and rotation
	FVector start = _cameraComponent->GetComponentLocation ();
	FVector end = _cameraComponent->GetComponentLocation () + (_cameraComponent->GetForwardVector () * 100000.0f);

	//Check if line trace hits anything
	if (GetWorld ()->LineTraceSingleByChannel (hit, start, end, ECC_Visibility, traceParams))
		return hit.Location;

	return _cameraComponent->GetComponentLocation () + (_cameraComponent->GetForwardVector () * 5000.0f);
}

void AMainCharacterController::HitPolymorph ()
{
	if (isPig)
		return;

	polymorphCharge += GetWorld ()->DeltaTimeSeconds;

	if (polymorphCharge >= 1.0f)
	{
		polymorphCharge = 1.0f;
		TurnIntoPig ();
	}
}

void AMainCharacterController::TurnIntoPig ()
{
	if (_isPolymorphing)
		StopPolymorph ();

	pigChargeTimer = 0.0f;
	pigTimer = 20.0f;
	isPig = true;
	TurnIntoPigBP ();
}

void AMainCharacterController::TurnIntoHuman ()
{
	isPig = false;
	TurnIntoHumanBP ();
}

void AMainCharacterController::GetLifetimeReplicatedProps (TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps (OutLifetimeProps);

	DOREPLIFETIME (AMainCharacterController, polymorphCharge);
	DOREPLIFETIME (AMainCharacterController, isPig);
	DOREPLIFETIME (AMainCharacterController, pigTimer);

	DOREPLIFETIME (AMainCharacterController, showTargetPigTimer);
	DOREPLIFETIME (AMainCharacterController, targetPigTimer);

	DOREPLIFETIME (AMainCharacterController, pigCharging);
	DOREPLIFETIME (AMainCharacterController, pigChargeTimer);

	DOREPLIFETIME (AMainCharacterController, _isPolymorphing);

	DOREPLIFETIME (AMainCharacterController, _canShoot);

	DOREPLIFETIME (AMainCharacterController, isStunned);

	DOREPLIFETIME (AMainCharacterController, _gameTimers);

	DOREPLIFETIME (AMainCharacterController, playerOneScore);
	DOREPLIFETIME (AMainCharacterController, playerTwoScore);
	DOREPLIFETIME (AMainCharacterController, playerThreeScore);

	DOREPLIFETIME (AMainCharacterController, _playerIndex);
	DOREPLIFETIME (AMainCharacterController, gameStarted);

	DOREPLIFETIME (AMainCharacterController, laserColor);
}

//Called to bind functionality to input
void AMainCharacterController::SetupPlayerInputComponent (UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent (PlayerInputComponent);

	PlayerInputComponent->BindAction ("Polymorph", IE_Pressed, this, &AMainCharacterController::StartPolymorph);
	PlayerInputComponent->BindAction ("Polymorph", IE_Released, this, &AMainCharacterController::StopPolymorph);
	PlayerInputComponent->BindAction ("Shoot", IE_Pressed, this, &AMainCharacterController::ShootInput);
	PlayerInputComponent->BindAction ("Shoot", IE_Released, this, &AMainCharacterController::StopChargeInput);
}
