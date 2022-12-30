// Fill out your copyright notice in the Description page of Project Settings.


#include "WaveGamePlayerController.h"
#include "WaveGameInstance.h"
#include "Kismet/GameplayStatics.h"

#include "MasterView.h"
#include "TurretHead.h"

AWaveGamePlayerController::AWaveGamePlayerController()
{
}

void AWaveGamePlayerController::BeginPlay()
{
	UWaveGameInstance* CurrentGameInstance = Cast<UWaveGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (CurrentGameInstance)
	{
		CurrentGameInstance->InitiateUI();
	}
	ATurretHead* TurretPawn = Cast<ATurretHead>(this->GetPawn());
	if (TurretPawn)
	{
		OwningPawn = TurretPawn;
	}
}

void AWaveGamePlayerController::AddPitchInput(float Val)
{
	RotationInput.Pitch += !IsLookInputIgnored() ? Val * InputPitchScale : 0.f;
}

void AWaveGamePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWaveGamePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// below will handle only pause. unpause is handled in the Masterview. Because at that time focus is on the UI not Playercontroller
	FInputActionBinding& PauseBinding = InputComponent->BindAction("Pause", IE_Pressed, this, &AWaveGamePlayerController::BroadcastEscape);
	PauseBinding.bConsumeInput = true;
	PauseBinding.bExecuteWhenPaused = true;
}

void AWaveGamePlayerController::BroadcastEscape()
{
	OnPressEscape.Broadcast();
}

void AWaveGamePlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
	ATurretHead* TurretPawn = Cast<ATurretHead>(aPawn);
	if (TurretPawn)
	{
		OwningPawn = TurretPawn;
		OnPlayerReady.Broadcast();
	}
}
