// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnemyAI.generated.h"

UENUM(BlueprintType)
enum class EnemyState : uint8
{
	IDLE = 0 UMETA(DisplayName = "IDLE"),
	MOVING = 1 UMETA(DisplayName = "MOVING"),
	DEAD = 2 UMETA(DisplayName = "DEAD"),
	ATTACK = 3 UMETA(DisplayName = "ATTACK"),
	STUNNED = 4 UMETA(DisplayName = "STUNNED")
};

UCLASS()
class WAVEGAME_API AEnemyAI : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemyAI();

	UPROPERTY(EditAnywhere, Category = "AI")
	class UBehaviorTree* BehaviorTree;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Mass")
	float MovementSpeed;

	UPROPERTY(VisibleAnywhere, Category = "Mass")
	class USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mass")
	class USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mass")
	class USphereComponent* SphereComponentArea;

	UPROPERTY(VisibleAnywhere, Category = "Mass")
	class UFloatingPawnMovement* PawnMovementComponent;

	EnemyState EnemyStatus;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	EnemyState GetEnemyStatus() { return EnemyStatus; };

	UFUNCTION(BlueprintCallable)
	class UFloatingPawnMovement* GetPawnMovementComponent() { return PawnMovementComponent; };
};
