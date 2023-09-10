// Fill out your copyright notice in the Description page of Project Settings.


#include "Common/HealthComponentBase.h"
#include "BasicProjectileDamage.h"
#include "Kismet/GameplayStatics.h"

#include "Player/TurretHead.h"
#include "AI/EnemyAI.h"
#include "Player/WaveGamePlayerController.h"

// Sets default values for this component's properties
UHealthComponentBase::UHealthComponentBase()
{

	DefaultHealth = 100;
	Health = DefaultHealth;
}


// Called when the game starts
void UHealthComponentBase::BeginPlay()
{
	Super::BeginPlay();

	/*
		all components have their own owners
		subscribing to owner's damage event
	*/
	AActor* OwnOwner = GetOwner();
	if (OwnOwner)
	{
		OwnOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponentBase::HandleTakeAnyDamage);
	}
	
}

void UHealthComponentBase::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f)
	{
		return;
	}

	/*
	*	issue I got by normal casting
		https://stackoverflow.com/questions/48759558/a-value-of-type-const-char-cannot-be-used-to-initialize-an-entity-of-type-ch
		issue resolved by casting like below. Dont know the difference
		https://forums.unrealengine.com/t/c-casting-acharacter-child-pointer-to-aactor-does-not-work/405575
	*/
	/*
	UBasicProjectileDamage* CustomDamageType = (UBasicProjectileDamage*)DamageType;
	if (CustomDamageType)
	{
	}
	*/

	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);
	
	ATurretHead* PlayerRef = Cast<ATurretHead>(DamagedActor);
	AEnemyAI* EnemyRef = Cast<AEnemyAI>(DamagedActor);
	
	if (PlayerRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("Getting Damage"));
		PlayerRef->OnHealthUpdate.Broadcast(Health);
	}

	if (Health == 0.0f)
	{
		if (EnemyRef)
		{
			LetPlayerKnowEnemyKill(InstigatedBy);
			EnemyRef->Die();
		}

		if (PlayerRef)
		{
			PlayerRef->Die();
		}
	}

}

void UHealthComponentBase::ResetHealth()
{
	Health = DefaultHealth;
	ATurretHead* PlayerRef = Cast<ATurretHead>(GetOwner());
	if (PlayerRef)
	{
		PlayerRef->OnHealthUpdate.Broadcast(Health);
	}
}

void UHealthComponentBase::LetPlayerKnowEnemyKill(AController* InstigatedBy)
{
	//UE_LOG(LogTemp, Error, TEXT("DAMAGE INSTIGATED BY %s"), *InstigatedBy->GetName());
	AWaveGamePlayerController* CurrentPlayerController = Cast<AWaveGamePlayerController>(InstigatedBy);
	if (CurrentPlayerController)
	{
		CurrentPlayerController->OwningPawn->OnPlayerScored.Broadcast(false);
	}

}
