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
		_gameMode = Cast <AMainGameMode> (GetWorld ()->GetAuthGameMode ());
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

		_gameTimer = _gameMode->gameTimer;
	}
	else
	{
		int minutes = FMath::FloorToInt (_gameTimer / 60);
		int seconds = FMath::RoundToInt ((int) _gameTimer % 60);


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
	}
}

void AMainCharacterController::StartPolymorph_Implementation ()
{
	if (isPig)
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
	if (isPig)
		return;

	_isPolymorphing = false;
	StopPolymorphBP ();
}

bool AMainCharacterController::StopPolymorph_Validate ()
{
	return true;
}

void AMainCharacterController::Polymorph ()
{
	if (isPig)
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
			return;

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
			}
		}
	}
	else
	{
		laserTargetPosition = _cameraComponent->GetComponentLocation () + (_cameraComponent->GetForwardVector () * 5000.0f);
	}
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

	polymorphCharge += 0.5f * GetWorld ()->DeltaTimeSeconds;

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

	DOREPLIFETIME (AMainCharacterController, _gameTimer);
}

//Called to bind functionality to input
void AMainCharacterController::SetupPlayerInputComponent (UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent (PlayerInputComponent);

	PlayerInputComponent->BindAction ("Polymorph", IE_Pressed, this, &AMainCharacterController::StartPolymorph);
	PlayerInputComponent->BindAction ("Polymorph", IE_Released, this, &AMainCharacterController::StopPolymorph);
}
