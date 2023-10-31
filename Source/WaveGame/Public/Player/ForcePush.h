// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ForcePush.generated.h"

UCLASS()
class WAVEGAME_API AForcePush : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AForcePush();

	UPROPERTY(VisibleAnywhere, Category = "Custom")
	class USphereComponent* SphereComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	float PushBackUnits;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	float InitialRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	float SpawnedLifeTime;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	float EnemyStunDuration;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UFUNCTION()
	void StartPush(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
