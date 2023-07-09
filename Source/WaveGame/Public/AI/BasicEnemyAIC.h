// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BasicEnemyAIC.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDirectionUpdate, FVector, StartLocation, FVector, EndLocation);

/**
 * 
 */
UCLASS()
class WAVEGAME_API ABasicEnemyAIC : public AAIController
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly, Category = "Behavior")
	float AttackRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Behavior")
	float TargetPointReachThreshold;

	UPROPERTY(BlueprintReadOnly)
	class AEnemyAI* CurrentPawn;

	UPROPERTY()
	FOnDirectionUpdate OnDirectionUpdate;

	UPROPERTY(EditDefaultsOnly, Category = "Behavior")
	float MovementSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Behavior")
	float TurnSpeed;

	AActor* CurrentTargetActor;

	FVector CurrentTargetActorLoc;

	FVector LastStartingLocation;

	FVector NextTargetPoint;

	bool bPushingBack;

	FVector PushedBackToPoint;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBlackboardComponent* BlackBoardComponent;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTreeComponent* BehaviorTreeComponent;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TargetLocationKey;

protected:

	FTimerHandle TimerHandle_EnemyAttack;

	FTimerHandle TimerHandle_PushBackRestTimer;


protected:

	float GetSelfToTargetPointLength(FVector TargetPoint);

	virtual void BeginPlay() override;

public:

	ABasicEnemyAIC();

	virtual void OnPossess(APawn* InPawn) override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void FindAndMakeTarget(float DeltaTime);

	void UpdateEnemyLookRotation(float DeltaTime);

	FVector CalculateNextPathPoint();

	UFUNCTION(BlueprintCallable)
	void MovePawnManually(float DeltaTime);

	UFUNCTION()
	void UpdateNextPath(FVector StartLocation, FVector EndLocation);

	UFUNCTION(BlueprintCallable)
	void AttackTarget();

	UFUNCTION(BlueprintCallable)
	void StartAttack();

	bool CheckIfNextTargetPointReached();

	void PushBack(FVector NewPosition);

	UFUNCTION()
	void ResetPushBack();

};
