// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAI.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/FloatingPawnMovement.h" 

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

