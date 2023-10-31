// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "WaveGameInstance.generated.h"

/**
 * https://www.youtube.com/watch?v=XKNIQga3HYk - init UI
 */
UCLASS()
class WAVEGAME_API UWaveGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UWaveGameInstance(const FObjectInitializer& ObjectIntializer);

	TSubclassOf<class UMasterView> MasterViewRef;
	
	class UMasterView* SpawnedMasterView;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UWaveGameManagerHub> ManagerHubClass;

	UPROPERTY()
	class UWaveGameManagerHub* ManagerHub = nullptr;

protected:

	virtual void PostInitProperties();

public:

	void InitiateUI();

	/**
	* Get the manager hub object that contains all the managers.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
	class UWaveGameManagerHub* GetManagerHub() const { return ManagerHub; }
	
};
