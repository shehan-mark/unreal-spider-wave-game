// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WaveGameManagerBase.h"
#include "EnemyManager.generated.h"

/**
 * Manager to maintain shared enemy knowledge
 */
UCLASS()
class WAVEGAME_API UEnemyManager : public UWaveGameManagerBase
{
	GENERATED_BODY()
	
protected:

	UPROPERTY()
	TMap<FString, FVector> EnemyTargetLocations;

public:

	UFUNCTION(BlueprintCallable)	
	void SetEnemyTargetLocation(FString EnemyOName, FVector TargetLocation);

	UFUNCTION(BlueprintCallable)
	bool IsLocationAvailable(FString QuerierName, FVector TestingTargetLocation);
};
