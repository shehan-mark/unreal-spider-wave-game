// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_LookATTargetPoint.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "AISystem.h"
#include "Kismet/KismetMathLibrary.h" 
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "WaveGameInstance.h"
#include "AI/EnemyManager.h"
#include "WaveGameManagerHub.h"

#include "AI/EnemyAI.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BTTask_LookATTargetPoint)

UBTTask_LookATTargetPoint::UBTTask_LookATTargetPoint(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// enable ticking
	bNotifyTick = true;

	NodeName = "Look At Target";
	TurnSpeed = 0.03f;

	// accept only actors and vectors
	//BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_LookATTargetPoint, BlackboardKey), AActor::StaticClass());
	//BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_LookATTargetPoint, BlackboardKey));

	TargetLocation = FVector(0, 0, 0);
}

EBTNodeResult::Type UBTTask_LookATTargetPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	Pawn = Cast<AEnemyAI>(Controller->GetPawn());

	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;
	if (Controller && Pawn)
	{
		UpdateLookAtTask(OwnerComp, NodeMemory);
	}
	else
	{
		NodeResult = EBTNodeResult::Failed;
	}


	return NodeResult;
}

EBTNodeResult::Type UBTTask_LookATTargetPoint::UpdateLookAtTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// starting to turn
	Pawn->SetIsTurning(true);
	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	AAIController* Controller = OwnerComp.GetAIOwner();
	
	// put task into InProgress mode
	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;
	if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		TargetLocation = Blackboard->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
		// Updating enemy manager with Pawn's data
		UWaveGameInstance* GameInstance = Cast<UWaveGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (GameInstance)
		{
			UEnemyManager* EnemyManager = GameInstance->GetManagerHub()->GetManager<UEnemyManager>();
			EnemyManager->SetEnemyTargetLocation(Pawn->GetName(), TargetLocation);
		}
		//DrawDebugDirectionalArrow(GetWorld(), Pawn->GetActorLocation(), TargetLocation, 5.0f, FColor::Red, true, 3.0f);
	}
	else if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		auto Value = Blackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
		AActor* TargetActor = Cast<AActor>(Value);
		FVector TargetActorLocation = FVector(TargetActor->GetActorLocation().X, TargetActor->GetActorLocation().Y, 50.0f);

		// I might want to set this target location in the Enemy Manager to avoid all enemies move to it anyway.
		// But for now its fine to keep it like this.
		TargetLocation = TargetActorLocation;
	}
	else
	{
		NodeResult = EBTNodeResult::Failed;
	}

	return NodeResult;
}

void UBTTask_LookATTargetPoint::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	check(GEngine != nullptr);
	
	if (TargetLocation.IsNearlyZero()) return;

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, TEXT("---------------------------------------"));

	// How much rotation needs to face the target.
	FVector PawnDirectionVector = Pawn->GetActorForwardVector() + Pawn->GetActorLocation();
	//DrawDebugDirectionalArrow(GetWorld(), PawnDirectionVector, TargetLocation, 5.0f, FColor::Blue, true, 3.0f);
	
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(PawnDirectionVector, TargetLocation);
	Pawn->SetActorRotation(FMath::Lerp(Pawn->GetController()->GetControlRotation(), LookAtRotation, TurnSpeed));

	// Target direction from the Spider towards the TargetLocation.
	FVector TargetDirection = TargetLocation - Pawn->GetActorLocation();
	TargetDirection.Normalize();
	
	float AngleDotProduct = FVector::DotProduct(Pawn->GetActorForwardVector(), TargetDirection);
	float AngleDegrees = FMath::RadiansToDegrees(acosf(AngleDotProduct));

	//FString AngleDegreesString = TEXT("Angle - ") + FString::SanitizeFloat(AngleDegrees);
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, *AngleDegreesString);
	//FString DotProductString = TEXT("DotProduct - ") + FString::SanitizeFloat(AngleDotProduct);
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, DotProductString);

	if (AngleDotProduct <= 1 && AngleDotProduct > 0.9)
	{
		// Finish task
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Finishing The Task"));
		Pawn->SetIsTurning(false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	
}