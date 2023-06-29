// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyManager.h"

void UEnemyManager::SetEnemyTargetLocation(FString EnemyOName, FVector TargetLocation)
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy Data Share Point %s, Location %f, %f, %f"), *EnemyOName, TargetLocation.X, TargetLocation.Y, TargetLocation.Z);
	EnemyTargetLocations.Add(EnemyOName, TargetLocation);
}

bool UEnemyManager::IsLocationAvailable(FString QuerierName, FVector TestingTargetLocation)
{
	bool _IsLocationAvailable = true;
	for (auto& Record : EnemyTargetLocations)
	{
		if (Record.Key == QuerierName) continue;
		FVector DistanceVector = Record.Value - TestingTargetLocation;
		float Distance = DistanceVector.Size();
		if (Distance <= 100.f)
		{
			_IsLocationAvailable = false;
			break;
		}
	}

	return _IsLocationAvailable;
}
