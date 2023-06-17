// Fill out your copyright notice in the Description page of Project Settings.


#include "ForcePush.h"
#include "EnemyAIBase.h"
#include "BasicEnemyAIC.h"

#include "Components/SphereComponent.h"

// Sets default values
AForcePush::AForcePush()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));

	InitialRadius = 200.f;
	SphereComponent->InitSphereRadius(InitialRadius);
	/*SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);*/
	SphereComponent->SetSimulatePhysics(false);
	RootComponent = SphereComponent;

	PushBackUnits = 300.f;
	SpawnedLifeTime = 1.0f;
	EnemyStunDuration = 3.0f;
}

// Called when the game starts or when spawned
void AForcePush::BeginPlay()
{
	Super::BeginPlay();
	
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AForcePush::StartPush);

	this->SetLifeSpan(SpawnedLifeTime);
}

void AForcePush::StartPush(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnemyAIBase* HitEnemy = Cast<AEnemyAIBase>(OtherActor);
	if (HitEnemy)
	{
		FVector EnemyCurentLocation = OtherActor->GetActorLocation();
		FVector DamageCauserLocation = GetActorLocation();

		FVector PushActorLocation = GetActorLocation();

		FVector Dir = EnemyCurentLocation - FVector(PushActorLocation.X, PushActorLocation.Y, EnemyCurentLocation.Z);
		Dir.Normalize();

		Dir *= PushBackUnits;
		FVector NewPos = EnemyCurentLocation + FVector(Dir.X, Dir.Y, 0.0f);

		ABasicEnemyAIC* EnemyAIC = Cast<ABasicEnemyAIC>(HitEnemy->GetController());
		if (EnemyAIC)
		{
			EnemyAIC->PushBack(NewPos);
		}
		//HitEnemy->Stun(EnemyStunDuration);
	}
}

