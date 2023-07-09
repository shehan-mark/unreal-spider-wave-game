// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SubViewBase.h"
#include "GameOverView.generated.h"

/**
 * 
 */
UCLASS()
class WAVEGAME_API UGameOverView : public USubViewBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* ScoreTextBlock;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* MainMenuButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* ScoreTextBlock_Value;

protected:

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnMainMenuPressed(); 
};
