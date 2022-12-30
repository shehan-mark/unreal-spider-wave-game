// Fill out your copyright notice in the Description page of Project Settings.

#include "WaveGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "EnemyAIBase.h"
#include "BasicProjectile.h"
#include "TurretHead.h"
#include "WaveGamePlayerController.h"

AWaveGameMode::AWaveGameMode()
{
	TimeBetweenWaves = 2.0f;
	SpawnCircleRadius = 1000.0f;
	MaxWaveCount = 5;
	EnemyWaveMultiplier = 2;

	PrimaryActorTick.TickInterval = 1.0f;
	PrimaryActorTick.bCanEverTick = true;

}

void AWaveGameMode::StartGame()
{
	TArray<AActor*> PlayerTurret;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATurretHead::StaticClass(), PlayerTurret);
	if (PlayerTurret.Num() == 0 && IsValid(PlayerTurretToSpawn))
	{
		FActorSpawnParameters SpawnParams;
		ATurretHead* NewTurretHead = GetWorld()->SpawnActor<ATurretHead>(PlayerTurretToSpawn, FVector(0.0f, 0.0f, 125.0f), FRotator(0.0f, 0.0f, 0.0f), SpawnParams);
		
		AWaveGamePlayerController* CurrentPlayerController = Cast<AWaveGamePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		
		// it seems I dont have to call posses because this PreInitializeComponents method is being called as soon as I spawn the Pawn.
		/*CurrentPlayerController->Possess(NewTurretHead);*/
	}

	PrepareForNextWave();
}

void AWaveGameMode::StartPlay()
{
	Super::StartPlay();

	// for testing
	//PrepareForNextWave();
}

void AWaveGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckWaveState();
}

void AWaveGameMode::PrepareForNextWave()
{
	WaveStatus = WaveGameModeState::PREWAVE;

	FTimerHandle TimerHandle_NextWaveStart;
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &AWaveGameMode::StartWave, TimeBetweenWaves, false);	
}

/*
*
* Only will run logic when wave is in progress
**/
void AWaveGameMode::CheckWaveState()
{
	if (WaveStatus != WaveGameModeState::WAVEINPROGRESS)
		return;

	bool bIsAnyEnemyAlive = false;
	TArray<AActor*> EnemiesInWorld;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyAIBase::StaticClass(), EnemiesInWorld);

	if (EnemiesInWorld.Num() > 0)
	{
		for (int i = 0; i < EnemiesInWorld.Num(); i++)
		{
			AEnemyAIBase* Enemy = Cast<AEnemyAIBase>(EnemiesInWorld[i]);
			if (Enemy && Enemy->GetEnemyStatus() != EnemyState::DEAD)
			{
				bIsAnyEnemyAlive = true;
				break;
			}
		}
	}

	/*for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		APawn* _Pawn = It->Get();
		AEnemyAIBase* Enemy = Cast<AEnemyAIBase>(_Pawn);
		if (Enemy && Enemy->GetEnemyStatus() != EnemyState::DEAD)
		{
			bIsAnyEnemyAlive = true;
			break;
		}
	}*/

	if (!bIsAnyEnemyAlive)
	{
		EndWave();
	}
	
}

void AWaveGameMode::SpawnNewEnemy()
{
	if (IsValid(SpawnEnemy))
	{
		FActorSpawnParameters SpawnParams;

		// finding random location/ point in a circle to spawn enemies.
		float Theta = FMath::DegreesToRadians(FMath::RandRange(0.0f, 359.0f));
		float XCoordinate = FMath::Cos(Theta) * SpawnCircleRadius;
		float YCoordinate = FMath::Sin(Theta) * SpawnCircleRadius;

		AEnemyAIBase* SpawnedEnemy = GetWorld()->SpawnActor<AEnemyAIBase>(SpawnEnemy, FVector(XCoordinate, YCoordinate, 20.0f), FRotator(0.0f, 0.0f, 0.0f), SpawnParams);

		// maintain of enemy loop game logic
		NumOfEnemiesToSpawn--;

		if (NumOfEnemiesToSpawn <= 0)
		{
			// done spawning for the current wave
			GetWorldTimerManager().ClearTimer(TimerHandle_EnemySpawner);
			WaveStatus = WaveGameModeState::WAVEINPROGRESS;
		}
	}
}

void AWaveGameMode::StartWave()
{
	EnemyWaveCount++;

	NumOfEnemiesToSpawn = EnemyWaveMultiplier * EnemyWaveCount;

	WaveStatus = WaveGameModeState::WAVESPAWNING;
	GetWorldTimerManager().SetTimer(TimerHandle_EnemySpawner, this, &AWaveGameMode::SpawnNewEnemy, 1.0f, true, 0.0f);
}

void AWaveGameMode::EndWave()
{
	WaveStatus = WaveGameModeState::WAVEENDED;

	if (EnemyWaveCount >= MaxWaveCount)
	{
		WaveStatus = WaveGameModeState::GAMEOVER;
	}
	else
	{
		PrepareForNextWave();
	}
}


void AWaveGameMode::DestroyAndStartOver()
{
	// this array has to be the type of AActor because GetAllActorsOfClass is not a template function
	// so cannot pass the exact AEnemyAIBase type. Will have to cast on iteration
	TArray<AActor*> EnemyAIs;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyAIBase::StaticClass(), EnemyAIs);
	for (AActor* TActor : EnemyAIs)
	{
		AEnemyAIBase* Enemy = Cast<AEnemyAIBase>(TActor);

		if (Enemy != nullptr)
		{
			Enemy->Destroy();
		}
	}
	// find and destroy existing projectiles
	TArray<AActor*> Projectiles;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABasicProjectile::StaticClass(), Projectiles);
	for (AActor* TActor : Projectiles)
	{
		ABasicProjectile* Projectile = Cast<ABasicProjectile>(TActor);

		if (Projectile != nullptr)
		{
			Projectile->Destroy();
		}
	}
	// Resetting Values and Timers
	EnemyWaveCount = 0;
	GetWorldTimerManager().ClearTimer(TimerHandle_EnemySpawner);

	// Finally start game
	StartGame();
}

void AWaveGameMode::RestartPlayer(AController* NewPlayer)
{
	// I dont this parent logic to run.
	// Since I'm manually spawning the player turret in StartGame
	return;
}

