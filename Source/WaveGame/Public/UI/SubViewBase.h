// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SubViewBase.generated.h"

/**
 * 
 */
UCLASS()
class WAVEGAME_API USubViewBase : public UUserWidget
{
	GENERATED_BODY()

public:

	class UMasterView* ParentViewRef;

public:


protected:

	virtual void NativePreConstruct() override;
	
};
