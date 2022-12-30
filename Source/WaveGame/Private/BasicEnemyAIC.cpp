// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicEnemyAIC.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"

#include "NavigationSystem.h" 
#include "NavigationPath.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h" 
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"

#include "EnemyAIBase.h"
#include "TurretHead.h"


ABasicEnemyAIC::ABasicEnemyAIC()
{
	//BlackBoardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoardComponent"));

	// this is what tells our behavior tree what to do
	//BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));

	TargetPointReachThreshold = 100.f;
	AttackRadius = 150.0f;
	MovementSpeed = 150.f;
	
	bPushingBack = false;
}

void ABasicEnemyAIC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//BlackBoardComponent->SetValueAsEnum(FName("EnemyState"), (uint8)CurrentPawn->GetEnemyStatus());

	FindAndMakeTarget(DeltaTime);

	if (bPushingBack)
	{
		CurrentPawn->SetActorLocation(FMath::Lerp(CurrentPawn->GetActorLocation(), PushedBackToPoint, 0.02f));
	}
	
}

void ABasicEnemyAIC::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (InPawn != nullptr)
	{
		CurrentPawn = Cast<AEnemyAIBase>(InPawn);
		
	}
}

void ABasicEnemyAIC::BeginPlay()
{
	Super::BeginPlay();

	OnDirectionUpdate.AddDynamic(this, &ABasicEnemyAIC::UpdateNextPath);
}

/*
* First find a target/ player
* Find the next path point towards target
* Look at the target/ player
* Start moving
*/
void ABasicEnemyAIC::FindAndMakeTarget(float DeltaTime)
{
	if (bPushingBack) return;

	if (CurrentPawn->GetEnemyStatus() == EnemyState::DEAD) return;
	
	if (CurrentPawn->IsStunned()) return;


	TArray<AActor*> Turrets;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATurretHead::StaticClass(), Turrets);
	if (Turrets.Num() < 1) return; // no turrets exist

	CurrentTargetActor = Turrets[0];
	CurrentTargetActorLKP = CurrentTargetActor->GetActorLocation();

	float EnemyToAttackTargetDistance = GetEnemyToTargetPointLength(CurrentTargetActorLKP);
	//UE_LOG(LogTemp, Error, TEXT("EnemyToAttackTargetDistance, %f"), EnemyToAttackTargetDistance);
	bool bCurrentTargetReached = EnemyToAttackTargetDistance < AttackRadius;

	// check if current target is reached
	if (NextTargetPoint.IsZero() || CheckIfCurrentTargetPointReached())
	{
		LastStartingLocation = CurrentPawn->GetActorLocation();
		NextTargetPoint = CalculateNextPathPoint();
	}

	if (bCurrentTargetReached)
	{
		NextTargetPoint = FVector(CurrentTargetActorLKP.X, CurrentTargetActorLKP.Y, CurrentPawn->GetActorLocation().Z);
		//DrawDebugDirectionalArrow(GetWorld(), CurrentPawn->GetActorLocation(), NextTargetPoint, 25.0f, FColor::Blue, true, 3.0f);
	}
	
	UpdateEnemyLookRotation(DeltaTime);
	MovePawnManually(DeltaTime);
}

/*
*  Turn pawn's forward direction towards the NextTargetPoint
*  https://forums.unrealengine.com/t/how-to-make-an-actor-rotate-to-look-in-a-certain-direction-in-c/144636/3
*  https://unrealcpp.com/open-door-with-lerp/
*/
void ABasicEnemyAIC::UpdateEnemyLookRotation(float DeltaTime)
{
	FVector CurrentEnemyLocation = CurrentPawn->GetActorLocation();
	FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(CurrentEnemyLocation, NextTargetPoint);
	CurrentPawn->SetActorRotation(FMath::Lerp(CurrentPawn->GetViewRotation(), LookRotation, 0.02f));
}

FVector ABasicEnemyAIC::CalculateNextPathPoint()
{
	/*
	*	I had to add Character.h header file in order for below function to understand that ACharacter is a type of Pawn.
	*	UNavigationPath*  NavigationPath = UNavigationSystemV1::FindPathToActorSynchronously(this, CurrentPawn->GetActorLocation(), PlayerPawn);
	*
	*/
	UNavigationPath* NavigationPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, LastStartingLocation, CurrentTargetActorLKP);

	if (NavigationPath->PathPoints.Num() > 1)
	{
		FVector PathPoint = LastStartingLocation;
		for (int i = 1; i < NavigationPath->PathPoints.Num(); i++)
		{
			//DrawDebugSphere(GetWorld(), NavigationPath->PathPoints[i], 20, 5, FColor(181, 0, 0), true, -1, 0, 2);
			PathPoint = FVector(NavigationPath->PathPoints[i].X, NavigationPath->PathPoints[i].Y, LastStartingLocation.Z);
			if (GetEnemyToTargetPointLength(PathPoint) > 50.f)
			{
				break;
			}
		}

		return PathPoint;
	}

	return CurrentPawn->GetActorLocation();
}

void ABasicEnemyAIC::MovePawnManually(float DeltaTime)
{
	if (!CheckIfCurrentTargetPointReached())
	{
		CurrentPawn->SetEnemyStatus(EnemyState::MOVING);
		FVector DiffVector = NextTargetPoint - CurrentPawn->GetActorLocation();
		float Magnitude = FMath::Sqrt(FMath::Pow(DiffVector.X, 2) + FMath::Pow(DiffVector.Y, 2) + FMath::Pow(DiffVector.Y, 2));
		FVector NormalizedVector = FVector(DiffVector.X / Magnitude, DiffVector.Y / Magnitude, DiffVector.Z / Magnitude);

		FVector NewLcoation = NormalizedVector * FVector(1, 1, 0) * MovementSpeed * DeltaTime;
		CurrentPawn->AddActorWorldOffset(NewLcoation);
	}
}

void ABasicEnemyAIC::UpdateNextPath(FVector StartLocation, FVector EndLocation)
{
	LastStartingLocation = StartLocation;
	NextTargetPoint = EndLocation;
}

bool ABasicEnemyAIC::CheckIfCurrentTargetPointReached()
{

	float LengthToCurrentTargetPoint = GetEnemyToTargetPointLength(NextTargetPoint);
	if (LengthToCurrentTargetPoint < TargetPointReachThreshold) return true;
	return false;
}

void ABasicEnemyAIC::PushBack(FVector NewPosition)
{
	bPushingBack = true;
	PushedBackToPoint = NewPosition;
	GetWorldTimerManager().SetTimer(TimerHandle_PushBackRestTimer, this, &ABasicEnemyAIC::ResetPushBack, 1.f, false, 2.0f);
}

void ABasicEnemyAIC::ResetPushBack()
{
	bPushingBack = false;
}

void ABasicEnemyAIC::StartAttack()
{
	CurrentPawn->SetEnemyStatus(EnemyState::ATTACK);
	GetWorldTimerManager().SetTimer(TimerHandle_EnemyAttack, this, &ABasicEnemyAIC::AttackTarget, CurrentPawn->AttackRate, true, 0.0f);
}

void ABasicEnemyAIC::AttackTarget()
{
	float DistanceToAttack = GetEnemyToTargetPointLength(FVector(CurrentTargetActorLKP.X, CurrentTargetActorLKP.Y, CurrentPawn->GetActorLocation().Z));
	bool bCloseEnoughToDoDamage = DistanceToAttack <= AttackRadius;
	if (bCloseEnoughToDoDamage && CurrentPawn->CurrentDamageTarget && CurrentPawn->CurrentDamageTarget->GetTurretStatus() != TurretState::DEAD)
	{
		CurrentPawn->DoDamage();
	}
	else
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_EnemyAttack);
	}
}

float ABasicEnemyAIC::GetEnemyToTargetPointLength(FVector TargetPoint)
{
	FVector CurrentEnemyLocation = CurrentPawn->GetActorLocation();
	FVector MovingDirection = (TargetPoint - CurrentEnemyLocation);
	float MovementMag = MovingDirection.Size();

	return MovementMag;
}
