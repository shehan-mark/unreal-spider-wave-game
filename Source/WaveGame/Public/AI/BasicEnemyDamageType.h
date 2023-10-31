// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "BasicEnemyDamageType.generated.h"

/**
 * 
 */
UCLASS()
class WAVEGAME_API UBasicEnemyDamageType : public UDamageType
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	float EnemyDamageAmount;
};
