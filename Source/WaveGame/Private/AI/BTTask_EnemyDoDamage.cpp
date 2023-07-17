// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_EnemyDoDamage.h"

#include "WaveGame/WaveGame.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "AI/BasicEnemyAIC.h"
#include "Player/TurretHead.h"

UBTTask_EnemyDoDamage::UBTTask_EnemyDoDamage(const FObjectInitializer& ObjectInitializer)
{
	NodeName = "Enemy Do Damage";

}

EBTNodeResult::Type UBTTask_EnemyDoDamage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	EBTNodeResult::Type NodeResult = EBTNodeResult::Failed;
	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	ABasicEnemyAIC* EnemyAIController = Cast<ABasicEnemyAIC>(OwnerComp.GetAIOwner());

	ATurretHead* TurretHead = Cast<ATurretHead>(Blackboard->GetValueAsObject(BB_KEY_ENEMY_TARGET_ACTOR));
	if (TurretHead)
	{
		bool Status = EnemyAIController->AttackTarget(TurretHead);
		if (Status) NodeResult = EBTNodeResult::Succeeded;
	}

	return NodeResult;
}
