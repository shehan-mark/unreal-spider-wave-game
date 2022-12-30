// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameHUD.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"

#include "WaveGamePlayerController.h"
#include "TurretHead.h"
#include "Components/TextBlock.h" 
#include "MasterView.h"


void UInGameHUD::NativeConstruct()
{
	Super::NativeConstruct();
	// Initial health status
	HealthBar_HUD->SetPercent(1);
	AbilityBar_HUD->SetPercent(0);

	CurrentPlayerController = Cast<AWaveGamePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (CurrentPlayerController)
	{
		CurrentPlayerController->OnPlayerReady.AddDynamic(this, &UInGameHUD::BindPlayerEvents);
	}
}

void UInGameHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	FText TextScore = FText::FromString(FString::SanitizeFloat(CurrentPlayerController->OwningPawn->GetScore()));
	TextBlock_ScoreValue->SetText(TextScore);
}

void UInGameHUD::UpdateHealthBar(float Health)
{
	float PercentageValue = Health / 100.f;
	HealthBar_HUD->SetPercent(PercentageValue);
}

void UInGameHUD::BindPlayerEvents()
{
	CurrentPlayerController = Cast<AWaveGamePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (CurrentPlayerController && CurrentPlayerController->OwningPawn)
	{
		CurrentPlayerController->OwningPawn->OnHealthUpdate.AddDynamic(this, &UInGameHUD::UpdateHealthBar);
		UpdateHealthBar(100.f);
		CurrentPlayerController->OwningPawn->OnAbilityAmountUpdate.AddDynamic(this, &UInGameHUD::UpdateAbilityBar);
	}
}

void UInGameHUD::UpdateAbilityBar(float AbilityAmount)
{
	float PercentageValue = AbilityAmount / 100.f;
	AbilityBar_HUD->SetPercent(PercentageValue);
}
