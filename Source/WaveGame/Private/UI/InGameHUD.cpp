// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGameHUD.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"

#include "Player/WaveGamePlayerController.h"
#include "Player/TurretHead.h"
#include "Components/TextBlock.h" 
#include "UI/MasterView.h"


void UInGameHUD::NativeConstruct()
{
	Super::NativeConstruct();
	// Initial health status
	HealthBar_HUD->SetPercent(1);
	AbilityBar_HUD->SetPercent(0);
}

void UInGameHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	/*FText TextScore = FText::FromString(FString::SanitizeFloat(CurrentPlayerController->OwningPawn->GetScore()));
	TextBlock_ScoreValue->SetText(TextScore);*/
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
		if (!CurrentPlayerController->OwningPawn->OnHealthUpdate.Contains(this, "UpdateHealthBar"))
		{
			CurrentPlayerController->OwningPawn->OnHealthUpdate.AddDynamic(this, &UInGameHUD::UpdateHealthBar);
		}
		if (!CurrentPlayerController->OwningPawn->OnHealthUpdate.Contains(this, "UpdateAbilityBar"))
		{
			CurrentPlayerController->OwningPawn->OnAbilityAmountUpdate.AddDynamic(this, &UInGameHUD::UpdateAbilityBar);
		}
	}
}

void UInGameHUD::UpdateAbilityBar(float AbilityAmount)
{
	float PercentageValue = AbilityAmount / 100.f;
	AbilityBar_HUD->SetPercent(PercentageValue);
}

void UInGameHUD::Init()
{
	Super::Init();
	
	BindPlayerEvents();
	/*CurrentPlayerController = Cast<AWaveGamePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (CurrentPlayerController && !CurrentPlayerController->OnPlayerReady.Contains(this, "BindPlayerEvents"))
	{
		UE_LOG(LogTemp, Warning, TEXT("------------------- BINDING PLAYER READY EVENT TO IN GAME HUD VIEW"));
		CurrentPlayerController->OnPlayerReady.AddDynamic(this, &UInGameHUD::BindPlayerEvents);
	}*/
}
