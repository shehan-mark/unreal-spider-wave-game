// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SubViewBase.h"
#include "PauseMenu.generated.h"

/**
 * 
 */
UCLASS()
class WAVEGAME_API UPauseMenu : public USubViewBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* ResumeButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* MainMenuButton;
	
protected:

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnResumeGamePressed();

	UFUNCTION()
	void OnMainMenuPressed();
};
