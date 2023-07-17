// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTDecorator_CloseEnough.generated.h"

/**
 * 
 */
UCLASS()
class WAVEGAME_API UBTDecorator_CloseEnough : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	float MinimumDistance;
	
protected:

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const;

public:

	UBTDecorator_CloseEnough();
};
