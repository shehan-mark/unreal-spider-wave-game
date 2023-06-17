// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIBase.h"
//#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "TimerManager.h"
#include "NavigationSystem.h" 
#include "NavigationPath.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

#include "HealthComponentBase.h"
#include "DrawDebugHelpers.h"
#include "BasicEnemyAIC.h"
#include "TurretHead.h"
#include "ForcePush.h"

// Sets default values
AEnemyAIBase::AEnemyAIBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));

	SphereComponent->InitSphereRadius(40.f);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereComponent->SetSimulatePhysics(false);
	RootComponent = SphereComponent;

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	SkeletalMeshComponent->SetCanEverAffectNavigation(false);
	SkeletalMeshComponent->SetupAttachment(SphereComponent);

	EnemyHealthComponent = CreateDefaultSubobject<UHealthComponentBase>(TEXT("EnemyHealthComponent"));

	LifeSpanAfterDeath = 3.0f;
	DamageAmount = 5.0f;
	AttackRate = 2.0f;

	Stunned = false;
}

// Called when the game starts or when spawned
void AEnemyAIBase::BeginPlay()
{
	Super::BeginPlay();
	//EnemyStatus = EnemyState::IDLE;

	//SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemyAIBase::OnCollisionOverlap);
}

void AEnemyAIBase::OnCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("AEnemyAIBase::OnCollisionOverlap - OtherActor - %s"), *OtherActor->GetName());
	ABasicEnemyAIC* AIC = Cast<ABasicEnemyAIC>(GetController());
	ATurretHead* CurrentTurret = Cast<ATurretHead>(OtherActor);
	if (CurrentTurret)
	{
		CurrentDamageTarget = CurrentTurret;
		if (AIC)
		{
			GetWorldTimerManager().ClearTimer(TimerHandle_StartAttack);
			// adding a little bit of delay before attack so we have time to play the turn animation towards the turret
			FTimerDelegate TimerDel;
			TimerDel.BindUFunction(this, FName("BeginAttack"), AIC);
			GetWorldTimerManager().SetTimer(TimerHandle_StartAttack, TimerDel, 1.0f, false, 1.0f);
		}
	}
	else
	{
		if (OtherActor == this) return;
		AForcePush* ForcePushActor = Cast<AForcePush>(OtherActor);
		if (ForcePushActor) return;

		FVector CurrLocation = GetActorLocation();
		FVector OtherActorLocation = OtherActor->GetActorLocation();

		FVector V1 = FVector(CurrLocation.X, CurrLocation.Y, 50.0f);
		FVector V2 = FVector(OtherActorLocation.X, OtherActorLocation.Y, 50.0f);

		FVector Dir = V1 - V2;
		Dir.Normalize();
		Dir *= 250.f;
		FVector NewPos = FVector(V1.X, V1.Y, CurrLocation.Z) + FVector(Dir.X, Dir.Y, 0.0);

		DrawDebugDirectionalArrow(GetWorld(), CurrLocation, NewPos, 5.0f, FColor::Red, true, 3.0f);

		if (AIC)
		{
			AIC->OnDirectionUpdate.Broadcast(CurrLocation, NewPos);
		}
	}
}

void AEnemyAIBase::BeginAttack(ABasicEnemyAIC* AIC)
{
	AIC->StartAttack();
}

// Called every frame
void AEnemyAIBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//void AEnemyAIBase::SetEnemyStatus(EnemyState Status)
//{
//	EnemyStatus = Status;
//}
//
//EnemyState AEnemyAIBase::GetEnemyStatus()
//{
//	return EnemyStatus;
//}

void AEnemyAIBase::DoDamage()
{
	ABasicEnemyAIC* Ctrl = Cast<ABasicEnemyAIC>(GetController());
	if (Ctrl)
	{
		//UE_LOG(LogTemp, Error, TEXT("AEnemyAIBase::DoDamage Damaging Player..."));
		UGameplayStatics::ApplyDamage(CurrentDamageTarget, DamageAmount, Ctrl, this, DamageType);
	}
}

void AEnemyAIBase::Die()
{
	//AController* APC = GetController();
	//if (APC)
	//{
	//	EnemyStatus = EnemyState::DEAD;
	//	//APC->UnPossess();
	//	APC->SetLifeSpan(LifeSpanAfterDeath);
	//	SetLifeSpan(LifeSpanAfterDeath);
	//}
}

//void AEnemyAIBase::Stun(float StunDuration)
//{
//	EnemyStatus = EnemyState::STUNNED;
//	GetWorldTimerManager().ClearTimer(TimerHandle_UnStun);
//	Stunned = true;
//	// start timer to unstun enemy
//	GetWorldTimerManager().SetTimer(TimerHandle_UnStun, this, &AEnemyAIBase::UnStun, 1.0f, false, StunDuration);
//}

//void AEnemyAIBase::UnStun()
//{
//	EnemyStatus = EnemyState::IDLE;
//	Stunned = false;
//}

bool AEnemyAIBase::IsStunned()
{
	return Stunned;
}
