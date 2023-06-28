// Fill out your copyright notice in the Description page of Project Settings.


#include "WaveGameInstance.h"

#include "MasterView.h"
#include "WaveGameManagerHub.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"

UWaveGameInstance::UWaveGameInstance(const FObjectInitializer& ObjectIntializer)
{
	/*
		this asset finding logic cannot be put anywhere else other than the constructor.
	*/
	static ConstructorHelpers::FClassFinder<UMasterView> MasterViewWidget(TEXT("/Game/Blueprints/UI/MasterView"));
	if (!ensure(MasterViewWidget.Class != nullptr))
	{
		return;
	}

	MasterViewRef = MasterViewWidget.Class;
}

void UWaveGameInstance::PostInitProperties()
{
	Super::PostInitProperties();

	// initializing the ManagerHub
	if (ManagerHubClass.Get())
	{
		ManagerHub = NewObject<UWaveGameManagerHub>(this, ManagerHubClass);
	}
}

void UWaveGameInstance::InitiateUI()
{
	SpawnedMasterView = CreateWidget<UMasterView>(this, MasterViewRef);
	SpawnedMasterView->AddToViewport();
}
