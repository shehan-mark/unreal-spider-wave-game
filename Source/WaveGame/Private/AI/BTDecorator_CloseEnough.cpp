// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator_CloseEnough.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "AI/BasicEnemyAIC.h"

UBTDecorator_CloseEnough::UBTDecorator_CloseEnough()
{
	NodeName = "Target Close Enough Check";

	MinimumDistance = 200.f;
}

bool UBTDecorator_CloseEnough::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	ABasicEnemyAIC* AIController = Cast<ABasicEnemyAIC>(OwnerComp.GetOwner());
	FVector OwnerLocation = AIController->GetPawn()->GetActorLocation();

	AActor* CurrentTargettedActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));
	FVector CurrentTargettedActorLocation = CurrentTargettedActor->GetActorLocation();
	CurrentTargettedActorLocation = FVector(CurrentTargettedActorLocation.X, CurrentTargettedActorLocation.Y, 50.f);

	if (CurrentTargettedActor)
	{
		FVector DistanceVector = CurrentTargettedActorLocation - OwnerLocation;
		if (DistanceVector.Size() < MinimumDistance)
		{
			return true;
		}
	}

	return false;
}
