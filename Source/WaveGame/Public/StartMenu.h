// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SubViewBase.h"
#include "StartMenu.generated.h"

/**
 * 
 */
UCLASS()
class WAVEGAME_API UStartMenu : public USubViewBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* StartButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* QuitButton;

protected:

protected:

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnStartGamePressed();

	UFUNCTION()
	void OnQuitGamePressed();
};
