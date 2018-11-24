// Fill out your copyright notice in the Description page of Project Settings.

#include "MainCharacterController.h"
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
}

//Called every frame
void AMainCharacterController::Tick (float DeltaTime)
{
	Super::Tick (DeltaTime);

	if (GetWorld ()->IsServer ())
	{
		if (_isPolymorphing)
			Polymorph ();
	}
}

void AMainCharacterController::StartPolymorph_Implementation ()
{
	_isPolymorphing = true;
}

bool AMainCharacterController::StartPolymorph_Validate ()
{
	return true;
}

void AMainCharacterController::StopPolymorph_Implementation ()
{
	_isPolymorphing = false;
}

bool AMainCharacterController::StopPolymorph_Validate ()
{
	return true;
}

void AMainCharacterController::Polymorph ()
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
	{
		//If line trace hits a player, polymorph the target
		if (hit.GetActor ()->ActorHasTag ("Player"))
		{
			Cast <AMainCharacterController> (hit.GetActor ())->TurnIntoPigBP ();
		}
		else
		{

		}
	}
	else
	{

	}
}

void AMainCharacterController::TurnIntoPig ()
{
	TurnIntoPigBP ();
}

void AMainCharacterController::TurnIntoHuman ()
{
	TurnIntoHumanBP ();
}

//Called to bind functionality to input
void AMainCharacterController::SetupPlayerInputComponent (UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent (PlayerInputComponent);

	PlayerInputComponent->BindAction ("Polymorph", IE_Pressed, this, &AMainCharacterController::StartPolymorph);
	PlayerInputComponent->BindAction ("Polymorph", IE_Released, this, &AMainCharacterController::StopPolymorph);
}
