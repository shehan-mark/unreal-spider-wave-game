// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTLookATTargetPoint.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "AISystem.h"
#include "Kismet/KismetMathLibrary.h" 
#include "DrawDebugHelpers.h"

#include "EnemyAI.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BTTLookATTargetPoint)

UBTTLookATTargetPoint::UBTTLookATTargetPoint(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// enable ticking
	bNotifyTick = true;

	NodeName = "Look At Target";
	TurnSpeed = 0.03f;

	// accept only actors and vectors
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTLookATTargetPoint, BlackboardKey), AActor::StaticClass());
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTLookATTargetPoint, BlackboardKey));
}

EBTNodeResult::Type UBTTLookATTargetPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

EBTNodeResult::Type UBTTLookATTargetPoint::UpdateLookAtTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	AAIController* Controller = OwnerComp.GetAIOwner();
	
	// put task into InProgress mode
	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;
	if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		TargetLocation = Blackboard->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
		DrawDebugDirectionalArrow(GetWorld(), Pawn->GetActorLocation(), TargetLocation, 5.0f, FColor::Red, true, 3.0f);
	}
	else
	{
		NodeResult = EBTNodeResult::Failed;
	}

	return NodeResult;
}

void UBTTLookATTargetPoint::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	check(GEngine != nullptr);
	
	// how much rotation needs to face the target.
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Pawn->GetActorLocation(), TargetLocation);
	Pawn->SetActorRotation(FMath::Lerp(Pawn->GetController()->GetControlRotation(), LookAtRotation, TurnSpeed));

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, TEXT("------------------- Running"));

	FVector PawnForwardVector = Pawn->GetActorForwardVector();
	FVector PawnForwardVectorTotal = Pawn->GetActorForwardVector() + Pawn->GetActorLocation();
	DrawDebugDirectionalArrow(GetWorld(), PawnForwardVectorTotal, TargetLocation, 5.0f, FColor::Blue, true, 3.0f);
	UE_LOG(LogTemp, Warning, TEXT("PawnForwardVector X - %f, Y - %f, Z - %f"), PawnForwardVector.X, PawnForwardVector.Y, PawnForwardVector.Z);
	UE_LOG(LogTemp, Error, TEXT("PawnForwardVectorTotal X - %f, Y - %f, Z - %f"), PawnForwardVectorTotal.X, PawnForwardVectorTotal.Y, PawnForwardVectorTotal.Z);
	//DrawDebugSphere(GetWorld(), Pawn->GetActorForwardVector(), 100, 5, FColor::Red, true, -1, 0, 2);
	//DrawDebugSphere(GetWorld(), TargetLocation, 20, 5, FColor::Blue, true, -1, 0, 2);

	float DotProduct = FVector::DotProduct(PawnForwardVectorTotal, TargetLocation);
	FString PrintStatement = TEXT("DotProduct - ") + FString::SanitizeFloat(DotProduct);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, PrintStatement);

	if (DotProduct > 0.0f)
	{
		// finish task
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Finishing The Task"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	
}