// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "BasicProjectileDamage.generated.h"

/**
 * https://forums.unrealengine.com/t/how-does-damagetype-work/85024/3
 * https://forums.unrealengine.com/t/how-to-create-and-use-damagetype/312240/4
 */
UCLASS()
class WAVEGAME_API UBasicProjectileDamage : public UDamageType
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	float ProjectileDamageAmount;
};
