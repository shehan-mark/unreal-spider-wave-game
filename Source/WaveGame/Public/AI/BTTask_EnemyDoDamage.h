// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_EnemyDoDamage.generated.h"

/**
 * 
 */
UCLASS()
class WAVEGAME_API UBTTask_EnemyDoDamage : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_EnemyDoDamage(const FObjectInitializer& ObjectInitializer);

private:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
