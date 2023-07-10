// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/EnemyAI.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

#include "AI/BasicEnemyAIC.h"

// Sets default values
AEnemyAI::AEnemyAI()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->InitSphereRadius(40.f);
	SphereComponent->SetCollisionProfileName(TEXT("BasicEnemy"));
	RootComponent = SphereComponent;
	
	SphereComponentArea = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponentArea"));
	SphereComponentArea->InitSphereRadius(120.f);
	SphereComponentArea->SetCollisionProfileName(TEXT("EnemyMass"));
	SphereComponent->SetupAttachment(RootComponent);

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	SkeletalMeshComponent->SetCanEverAffectNavigation(false);
	SkeletalMeshComponent->SetupAttachment(SphereComponent);

	MovementSpeed = 350.f;
	PawnMovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>("PawnMovementComponent");
	PawnMovementComponent->MaxSpeed = MovementSpeed;
	//PawnMovementComponent->turn
	bUseControllerRotationYaw = true;
	

	DamageAmount = 10.f;
	AfterLifeTime = 1.0f;
}

// Called when the game starts or when spawned
void AEnemyAI::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyAI::Attack()
{
	ABasicEnemyAIC* AIController = Cast<ABasicEnemyAIC>(GetController());
	AActor* CurrentTarget = Cast<AActor>(AIController->GetBlackboardComponent()->GetValueAsObject("TargetActor"));
	if (CurrentTarget)
	{
		UGameplayStatics::ApplyDamage(CurrentTarget, DamageAmount, AIController, this, DamageType);
	}
}

void AEnemyAI::Die()
{
	ABasicEnemyAIC* AIController = Cast<ABasicEnemyAIC>(GetController());
	if (AIController)
	{
		AIController->UnPossess();
		AIController->SetLifeSpan(AfterLifeTime);
		SetLifeSpan(AfterLifeTime);
	}
}

