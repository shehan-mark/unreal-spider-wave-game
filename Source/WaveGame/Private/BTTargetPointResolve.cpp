// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTargetPointResolve.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

#include "BasicEnemyAIC.h"
#include "TurretHead.h"

/*
**
* below will be the implementation of this behavior tree task
*/
EBTNodeResult::Type UBTTargetPointResolve::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ABasicEnemyAIC* AIC = Cast<ABasicEnemyAIC>(OwnerComp.GetAIOwner());

    /*
        If the Controller is valid:
        1) Get the Blackboard Component and the Current Point of the bot
        2) Search for the next point, which will be different from the Current Point
    */
    if (AIC)
    {

        UBlackboardComponent* BlackboardComp = AIC->BlackBoardComponent;
        //ABotTargetPoint* CurrentPoint = Cast<ABotTargetPoint>(BlackboardComp->GetValueAsObject("LocationToGo"));

        //TArray<AActor*> AvailableTargetPoints = AICon->GetAvailableTargetPoints();

        //This variable will contain a random index in order to determine the next possible point
        //int32 RandomIndex;

        //Here, we store the possible next target point
        //ABotTargetPoint* NextTargetPoint = nullptr;

        //Find a next point which is different from the current one
        //do
        //{
        //    RandomIndex = FMath::RandRange(0, AvailableTargetPoints.Num() - 1);
        //    //Remember that the Array provided by the Controller function contains AActor* objects so we need to cast.
        //    NextTargetPoint = Cast<ABotTargetPoint>(AvailableTargetPoints[RandomIndex]);
        //} while (CurrentPoint == NextTargetPoint);
        check(GEngine != nullptr);

        FVector TargetLocation(-190.0f, -67.0f, 51.f);
        TArray<AActor*> PlayerTurret;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATurretHead::StaticClass(), PlayerTurret);
        if (PlayerTurret.Num() != 0)
        {
            ATurretHead* Player = Cast<ATurretHead>(PlayerTurret[0]);
            FVector PlayerLocation = Player->GetActorLocation();
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Player Turret Found - Location - %f, %f, %f"), PlayerLocation.X - 50, PlayerLocation.Y - 50, 51.f));
            TargetLocation = FVector(PlayerLocation.X - 50, PlayerLocation.Y - 50, 51.f);
        }


        //Update the next location in the Blackboard so the bot can move to the next Blackboard value
        BlackboardComp->SetValueAsVector("TargetLocation", FVector(-190.f, -67.f, 51.f));
        BlackboardComp->SetValueAsObject("SelfActor", AIC->GetPawn());

        //At this point, the task has been successfully completed
        return EBTNodeResult::Succeeded;
    }
    return EBTNodeResult::Failed;
}