// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnemyAI.generated.h"

UENUM(BlueprintType)
enum class EnemyState : uint8
{
	WANDERING = 0 UMETA(DisplayName = "WANDERING"),
	CHASING = 1 UMETA(DisplayName = "CHASING"),
	ATTACK = 2 UMETA(DisplayName = "ATTACK"),
	DEAD = 3 UMETA(DisplayName = "DEAD"),
	STUNNED = 4 UMETA(DisplayName = "STUNNED")
};

UCLASS()
class WAVEGAME_API AEnemyAI : public APawn
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, Category = "AI")
	class UBehaviorTree* BehaviorTree;

protected:

	UPROPERTY(VisibleAnywhere, Category = "Mass")
	class USkeletalMeshComponent* SkeletalMeshComponent;

	// component to detect incoming damage and collision with the world.
	UPROPERTY(VisibleAnywhere, Category = "Mass")
	class USphereComponent* SphereComponent;

	// component to represent the area that the enemy takes in the world.
	UPROPERTY(VisibleAnywhere, Category = "Mass")
	class USphereComponent* SphereComponentArea;

	UPROPERTY(VisibleAnywhere, Category = "Mass")
	class UFloatingPawnMovement* PawnMovementComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Offense")
	TSubclassOf<class UDamageType> DamageType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	class UHealthComponentBase* EnemyHealthComponent;

	EnemyState EnemyStatus;

	// basic damage amount the enemy can inflict.
	UPROPERTY(EditDefaultsOnly, Category = "Offense")
	float DamageAmount;

	UPROPERTY(EditDefaultsOnly, Category = "Mass")
	float MovementSpeed;
	
	// how many seconds the enemy stay in the world after dying.
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	float AfterLifeTime;

	// to detect if the enemy is turning towards something or not.
	bool IsTurning;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Sets default values for this pawn's properties
	AEnemyAI();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	EnemyState GetEnemyStatus() { return EnemyStatus; };

	UFUNCTION(BlueprintCallable)
	void SetEnemyStatus(EnemyState State) { EnemyStatus = State; };

	UFUNCTION(BlueprintCallable)
	class UFloatingPawnMovement* GetPawnMovementComponent() { return PawnMovementComponent; };

	UFUNCTION()
	bool Damage(class ATurretHead* Target);

	UFUNCTION()
	void Die(AController* InstigatedBy);

	UFUNCTION(BlueprintCallable)
	void SetIsTurning(bool Turning) { IsTurning = Turning; };

	UFUNCTION(BlueprintCallable)
	bool GetIsTurning() { return IsTurning; };

	UFUNCTION()
	void LetPlayerKnowAboutDying(AController* InstigatedBy);
};
