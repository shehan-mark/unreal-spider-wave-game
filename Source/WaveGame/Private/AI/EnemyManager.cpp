// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EnemyManager.h"

void UEnemyManager::SetEnemyTargetLocation(FString EnemyOName, FVector TargetLocation)
{
	EnemyTargetLocations.Add(EnemyOName, TargetLocation);
}

bool UEnemyManager::IsLocationAvailable(FString QuerierName, FVector TestingTargetLocation)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("-----------------"));
	//PrintVectors(TEXT("In Location ") + QuerierName, TestingTargetLocation);
	bool _IsLocationAvailable = true;
	for (auto& Record : EnemyTargetLocations)
	{
		if (Record.Key == QuerierName) continue;
		FVector DistanceVector = Record.Value - TestingTargetLocation;
		float Distance = DistanceVector.Size();
		/*FString PrintStr = QuerierName + TEXT(", Can I move? - ") + FString::SanitizeFloat(Distance);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, PrintStr);*/
		if (Distance <= 100.f)
		{
			_IsLocationAvailable = false;
			break;
		}
	}

	return _IsLocationAvailable;
}

void UEnemyManager::PrintVectors(const FString Message, FVector VectorValue)
{
	FString PrintStr = Message + TEXT(" - ") + FString::SanitizeFloat(VectorValue.X) + TEXT(" - ") + FString::SanitizeFloat(VectorValue.Y) + TEXT(" - ") + FString::SanitizeFloat(VectorValue.Z);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, Message);
}
