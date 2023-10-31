// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"

#include "BTTask_LookATTargetPoint.generated.h"


/**
 *	check this out 
 *	https://docs.unrealengine.com/4.26/en-US/API/Runtime/AIModule/BehaviorTree/UBTTaskNode/
 */
UCLASS(config = Game)
class WAVEGAME_API UBTTask_LookATTargetPoint : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float TurnSpeed;

protected:
	class AEnemyAI* Pawn;

	FVector TargetLocation;

public:
	UBTTask_LookATTargetPoint(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	EBTNodeResult::Type UpdateLookAtTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

};
