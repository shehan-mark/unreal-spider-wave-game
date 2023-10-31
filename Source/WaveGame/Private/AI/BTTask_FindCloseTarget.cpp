// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_FindCloseTarget.h"

#include "WaveGame/WaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Player/TurretHead.h"
#include "AI/BasicEnemyAIC.h"
#include "AI/EnemyAI.h"

UBTTask_FindCloseTarget::UBTTask_FindCloseTarget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Find Closest Target";

	// filter to only accept actor class
	//BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindCloseTarget, BlackboardKey), AActor::StaticClass());
}

/*
*
* TODO - Will check the distance to the turret and the spider to pick best option to reach
*/
EBTNodeResult::Type UBTTask_FindCloseTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, TEXT("Executing Task"));
	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;

	ABasicEnemyAIC* AIController = Cast<ABasicEnemyAIC>(OwnerComp.GetAIOwner());
	AIController->UpdateStatus(EnemyState::WANDERING);

	/*AEnemyAI* Pawn = Cast<AEnemyAI>(AIController->GetPawn());
	Pawn->SetIsTurning(false);*/

	TArray<AActor*> TurretTargets;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATurretHead::StaticClass(), TurretTargets);
	if (TurretTargets.Num() != 0 && AIController)
	{
		UBlackboardComponent* BlackboardComp = AIController->BlackBoardComponent;
		BlackboardComp->SetValueAsObject(BB_KEY_ENEMY_TARGET_ACTOR, TurretTargets[0]);
		AIController->UpdateStatus(EnemyState::CHASING);
	}
	NodeResult = EBTNodeResult::Succeeded;

	return NodeResult;
}
