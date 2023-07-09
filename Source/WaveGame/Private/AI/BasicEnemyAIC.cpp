// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BasicEnemyAIC.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"

#include "NavigationSystem.h" 
#include "NavigationPath.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h" 
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"

//#include "EnemyAIBase.h"
#include "AI/EnemyAI.h"
#include "Player/TurretHead.h"


ABasicEnemyAIC::ABasicEnemyAIC()
{
	BlackBoardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoardComponent"));
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	TargetLocationKey = "TargetLocation";

	TargetPointReachThreshold = 50.f;
	AttackRadius = 190.0f;
	MovementSpeed = 110.f;
	TurnSpeed = 0.03f;
	
	bPushingBack = false;
}

void ABasicEnemyAIC::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	CurrentPawn = Cast<AEnemyAI>(InPawn);
	check(GEngine != nullptr);
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("TREE STARTED!!!!"));
	if (CurrentPawn)
	{
		if (CurrentPawn->BehaviorTree->BlackboardAsset)
		{
			// if behavior tree property of our possesing pawn is correct. We are initializing it and starting the Tree.
			BlackBoardComponent->InitializeBlackboard(*CurrentPawn->BehaviorTree->BlackboardAsset);
			BehaviorTreeComponent->StartTree(*CurrentPawn->BehaviorTree);
		}
	}
}

void ABasicEnemyAIC::BeginPlay()
{
	Super::BeginPlay();

	//OnDirectionUpdate.AddDynamic(this, &ABasicEnemyAIC::UpdateNextPath);
}

void ABasicEnemyAIC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//BlackBoardComponent->SetValueAsEnum(FName("EnemyState"), (uint8)CurrentPawn->GetEnemyStatus());

	/*FindAndMakeTarget(DeltaTime);

	if (bPushingBack)
	{
		CurrentPawn->SetActorLocation(FMath::Lerp(CurrentPawn->GetActorLocation(), PushedBackToPoint, TurnSpeed));
	}*/
	
}

/*
* 1. find a target/ player
* 2. Find the next path point towards target
* 3. Look at the target/ player
* 4. Start moving
*/
void ABasicEnemyAIC::FindAndMakeTarget(float DeltaTime)
{
	if (bPushingBack) return;

	//if (CurrentPawn->IsStunned()) return;

	// find/ get turrets in the level and pick one for the attack.
	TArray<AActor*> Turrets;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATurretHead::StaticClass(), Turrets);
	// no turrets exist
	if (Turrets.Num() < 1)
	{
		//CurrentPawn->SetEnemyStatus(EnemyState::IDLE);
		return;
	}

	EnemyState CurrentStatus = CurrentPawn->GetEnemyStatus();
	if (CurrentStatus == EnemyState::DEAD || CurrentStatus == EnemyState::ATTACK) return;

	CurrentTargetActor = Turrets[0];
	CurrentTargetActorLoc = CurrentTargetActor->GetActorLocation();
	float EnemyToAttackTargetDistance = GetSelfToTargetPointLength(CurrentTargetActorLoc);
	bool bCurrentTargetReached = EnemyToAttackTargetDistance <= AttackRadius;

	if (bCurrentTargetReached)
	{
		NextTargetPoint = FVector(CurrentTargetActorLoc.X, CurrentTargetActorLoc.Y, CurrentPawn->GetActorLocation().Z);
		UpdateEnemyLookRotation(DeltaTime);
		// no need to execute the rest.
		return;
	}

	// check if current target is reached
	// NextTargetPoint can be zero when self just spawned to the world. Hence the check for IsZero
	if (NextTargetPoint.IsZero() || CheckIfNextTargetPointReached())
	{
		LastStartingLocation = CurrentPawn->GetActorLocation();
		NextTargetPoint = CalculateNextPathPoint();
		//DrawDebugSphere(GetWorld(), NextTargetPoint, 20, 5, FColor(0, 180, 0), true, -1, 0, 2);
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
	FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(CurrentPawn->GetActorLocation(), NextTargetPoint);
	CurrentPawn->SetActorRotation(FMath::Lerp(CurrentPawn->GetViewRotation(), LookRotation, TurnSpeed));
}

FVector ABasicEnemyAIC::CalculateNextPathPoint()
{
	/*
	*	UNavigationPath*  NavigationPath = UNavigationSystemV1::FindPathToActorSynchronously(this, CurrentPawn->GetActorLocation(), PlayerPawn);
	*/
	UNavigationPath* NavigationPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, LastStartingLocation, CurrentTargetActorLoc);

	if (NavigationPath->PathPoints.Num() > 1)
	{
		FVector PathPoint = LastStartingLocation;
		for (int i = 1; i < NavigationPath->PathPoints.Num(); i++)
		{
			// DrawDebugSphere(GetWorld(), NavigationPath->PathPoints[i], 20, 5, FColor(181, 0, 0), true, -1, 0, 2);
			PathPoint = FVector(NavigationPath->PathPoints[i].X, NavigationPath->PathPoints[i].Y, LastStartingLocation.Z);
			if (GetSelfToTargetPointLength(PathPoint) > TargetPointReachThreshold)
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
	// if reached dont move till we get a new updated path point.
	if (CheckIfNextTargetPointReached()) return;

	//CurrentPawn->SetEnemyStatus(EnemyState::MOVING);
	FVector DiffVector = NextTargetPoint - CurrentPawn->GetActorLocation();
	float Magnitude = FMath::Sqrt(FMath::Pow(DiffVector.X, 2) + FMath::Pow(DiffVector.Y, 2) + FMath::Pow(DiffVector.Y, 2));
	FVector NormalizedVector = FVector(DiffVector.X / Magnitude, DiffVector.Y / Magnitude, DiffVector.Z / Magnitude);

	FVector NewLcoation = NormalizedVector * FVector(1, 1, 0) * MovementSpeed * DeltaTime;
	CurrentPawn->AddActorWorldOffset(NewLcoation);
}

void ABasicEnemyAIC::UpdateNextPath(FVector StartLocation, FVector EndLocation)
{
	LastStartingLocation = StartLocation;
	NextTargetPoint = EndLocation;
}

bool ABasicEnemyAIC::CheckIfNextTargetPointReached()
{
	float LengthToCurrentTargetPoint = GetSelfToTargetPointLength(NextTargetPoint);
	if (LengthToCurrentTargetPoint < TargetPointReachThreshold)
	{
		return true;
	}
	return false;
}

void ABasicEnemyAIC::PushBack(FVector NewPosition)
{
	bPushingBack = true;
	PushedBackToPoint = NewPosition;
	// start timer to reset pushing back
	GetWorldTimerManager().SetTimer(TimerHandle_PushBackRestTimer, this, &ABasicEnemyAIC::ResetPushBack, 1.f, false, 2.0f);
}

void ABasicEnemyAIC::ResetPushBack()
{
	bPushingBack = false;
}

void ABasicEnemyAIC::StartAttack()
{
	//UE_LOG(LogTemp, Warning, TEXT("STARTING TO ATTACK..."));
	//CurrentPawn->SetEnemyStatus(EnemyState::ATTACK);
	//GetWorldTimerManager().SetTimer(TimerHandle_EnemyAttack, this, &ABasicEnemyAIC::AttackTarget, CurrentPawn->AttackRate, true, 0.0f);
}

void ABasicEnemyAIC::AttackTarget()
{
	float DistanceToAttack = GetSelfToTargetPointLength(FVector(CurrentTargetActorLoc.X, CurrentTargetActorLoc.Y, CurrentPawn->GetActorLocation().Z));
	bool bCloseEnoughToDoDamage = DistanceToAttack <= AttackRadius;
	/*if (bCloseEnoughToDoDamage && CurrentPawn->CurrentDamageTarget && CurrentPawn->CurrentDamageTarget->GetTurretStatus() != TurretState::DEAD)
	{
		CurrentPawn->DoDamage();
	}
	else
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_EnemyAttack);
	}*/
}

float ABasicEnemyAIC::GetSelfToTargetPointLength(FVector TargetPoint)
{
	FVector CurrentSelfLocation = CurrentPawn->GetActorLocation();
	FVector MovingDirection = (TargetPoint - CurrentSelfLocation);
	float MovementMag = MovingDirection.Size();

	return MovementMag;
}
