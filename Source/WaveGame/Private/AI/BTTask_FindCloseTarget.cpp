// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_FindCloseTarget.h"
#include "Kismet/GameplayStatics.h"
#include "TurretHead.h"
#include "BasicEnemyAIC.h"
#include "BehaviorTree/BlackboardComponent.h"

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
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, TEXT("Executing Task"));
	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;

	ABasicEnemyAIC* AIController = Cast<ABasicEnemyAIC>(OwnerComp.GetAIOwner());

	TArray<AActor*> TurretTargets;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATurretHead::StaticClass(), TurretTargets);
	if (TurretTargets[0] != nullptr && AIController)
	{
		UBlackboardComponent* BlackboardComp = AIController->BlackBoardComponent;
		BlackboardComp->SetValueAsObject("TargetActor", TurretTargets[0]);

	}
	NodeResult = EBTNodeResult::Succeeded;

	return NodeResult;
}
