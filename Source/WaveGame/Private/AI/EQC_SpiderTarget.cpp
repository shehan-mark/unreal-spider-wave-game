// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EQC_SpiderTarget.h"

#include "WaveGame/WaveGame.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "AI/EnemyAI.h"
#include "AI/BasicEnemyAIC.h"

void UEQC_SpiderTarget::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);

	AActor* Pawn = Cast<AActor>(QueryInstance.Owner.Get());
	if (Pawn == nullptr) return;
	ABasicEnemyAIC* EnemyAIC = Cast<ABasicEnemyAIC>(Pawn->GetInstigatorController());

	if (EnemyAIC && EnemyAIC->BlackBoardComponent)
	{
		AActor* TargetActor = Cast<AActor>(EnemyAIC->BlackBoardComponent->GetValueAsObject(BB_KEY_ENEMY_TARGET_ACTOR));
		//Set the context SeeingPawn to the provided context data
		UEnvQueryItemType_Actor::SetContextHelper(ContextData, TargetActor);
	}
}
