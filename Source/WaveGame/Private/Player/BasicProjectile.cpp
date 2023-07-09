// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BasicProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyAIBase.h"
#include "BasicProjectileDamage.h"
#include "DrawDebugHelpers.h"
#include "Player/TurretHead.h"
#include "AI/BasicEnemyAIC.h"

// Sets default values
ABasicProjectile::ABasicProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->InitSphereRadius(16.0f);

	// Players can't walk on it
	SphereComponent->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	SphereComponent->CanCharacterStepUpOn = ECB_No;

	RootComponent = SphereComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetWorldScale3D(FVector(0.2f, 0.2f, 0.2f));
	MeshComponent->SetupAttachment(SphereComponent);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = SphereComponent;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 5000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	EnemyKnockbackAmount = 100.0f;
}

// Called when the game starts or when spawned
void ABasicProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	SphereComponent->OnComponentHit.AddDynamic(this, &ABasicProjectile::OnHit);

}

// Called every frame
void ABasicProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABasicProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	//UE_LOG(LogTemp, Warning, TEXT("ABasicProjectile::OnHit %s"), *OtherActor->GetName());
	AEnemyAIBase* HitEnemy = Cast<AEnemyAIBase>(OtherActor);
	if (HitEnemy)
	{
		/* Reduce velocity of the projectile to minimize the effect after hitting the enemy */
		ProjectileMovement->Velocity = FVector(500.0f, 0.0f, 0.0f);
		ProjectileMovement->UpdateComponentVelocity();

		APlayerController* Ctrl = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (Ctrl)
		{
			UGameplayStatics::ApplyDamage(HitEnemy, 50.0f, Ctrl, this, DamageType);
		}

		FVector EnemyCurentLocation = OtherActor->GetActorLocation();
		FVector DamageCauserLocation = ProjectileOwner->GetActorLocation();

		FVector Dir = EnemyCurentLocation - FVector(0.0f, 0.0f, EnemyCurentLocation.Z);
		Dir.Normalize();

		Dir *= EnemyKnockbackAmount;
		FVector NewPos = EnemyCurentLocation + FVector(Dir.X, Dir.Y, 0.0f);

		DrawDebugDirectionalArrow(GetWorld(), FVector(0.0f, 0.0f, EnemyCurentLocation.Z), NewPos, 5.0f, FColor::Red, true, 3.0f);

		ABasicEnemyAIC* EnemyAIC = Cast<ABasicEnemyAIC>(HitEnemy->GetController());
		if (EnemyAIC)
		{
			EnemyAIC->PushBack(NewPos);
		}
	}

}
