// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator_CloseEnough.h"

#include "WaveGame//WaveGame.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "AI/BasicEnemyAIC.h"
#include "AI/EnemyAI.h"

UBTDecorator_CloseEnough::UBTDecorator_CloseEnough()
{
	NodeName = "Target Close Enough Check";

	MinimumDistance = 220.f;
}

bool UBTDecorator_CloseEnough::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	ABasicEnemyAIC* AIController = Cast<ABasicEnemyAIC>(OwnerComp.GetOwner());
	FVector OwnerLocation = AIController->GetPawn()->GetActorLocation();

	AActor* CurrentTargettedActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BB_KEY_ENEMY_TARGET_ACTOR));
	FVector CurrentTargettedActorLocation = CurrentTargettedActor->GetActorLocation();
	CurrentTargettedActorLocation = FVector(CurrentTargettedActorLocation.X, CurrentTargettedActorLocation.Y, 50.f);
	
	//DrawDebugDirectionalArrow(GetWorld(), OwnerLocation, CurrentTargettedActorLocation, 5.0f, FColor::Red, true, 3.0f);
	
	if (CurrentTargettedActor)
	{
		FVector DistanceVector = CurrentTargettedActorLocation - OwnerLocation;
		if (DistanceVector.Size() <= MinimumDistance)
		{
			return true;
		}
		else
		{
			AIController->UpdateStatus(EnemyState::CHASING);
		}
	}

	return false;
}
