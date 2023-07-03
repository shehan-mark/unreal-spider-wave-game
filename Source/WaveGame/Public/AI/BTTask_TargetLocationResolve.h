// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_RunEQSQuery.h"
#include "UObject/ObjectMacros.h"

#include "BTTask_TargetLocationResolve.generated.h"

/**
 * 
 */
UCLASS()
class WAVEGAME_API UBTTask_TargetLocationResolve : public UBTTask_RunEQSQuery
{
	GENERATED_BODY()
	
protected:

	//UPROPERTY()
	FVector PreviousPickedLocation;

public:
	UBTTask_TargetLocationResolve(const FObjectInitializer& ObjectInitializer);

private:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	/** finish task */
	//UFUNCTION()
	void OnQueryFinished(TSharedPtr<FEnvQueryResult> Result);
};
