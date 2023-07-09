// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SubViewBase.h"
#include "InGameHUD.generated.h"

/**
 * 
 */
UCLASS()
class WAVEGAME_API UInGameHUD : public USubViewBase
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UProgressBar* HealthBar_HUD;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UProgressBar* AbilityBar_HUD;

	class AWaveGamePlayerController* CurrentPlayerController;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* TextBlock_ScoreValue;

public:

	UFUNCTION()
	void UpdateHealthBar(float Health);

	UFUNCTION()
	void BindPlayerEvents();

	UFUNCTION()
	void UpdateAbilityBar(float AbilityAmount);

protected:

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
