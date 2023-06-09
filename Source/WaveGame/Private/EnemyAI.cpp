// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAI.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "HealthComponentBase.h"

// Sets default values
AEnemyAI::AEnemyAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	MovementSpeed = 350.f;

	EnemyHealthComponent = CreateDefaultSubobject<UHealthComponentBase>(TEXT("EnemyHealthComponent"));
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	MovementComponent->MaxWalkSpeed = MovementSpeed;
	
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

