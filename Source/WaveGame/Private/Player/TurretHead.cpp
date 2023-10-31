// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TurretHead.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

#include "Common/HealthComponentBase.h"
#include "Player/BasicProjectile.h"
#include "WaveGameInstance.h"
#include "Player/ForcePush.h"
#include "Player/WaveGamePlayerController.h"

// Sets default values
ATurretHead::ATurretHead()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	GetCapsuleComponent()->InitCapsuleSize(42.f, 42.f);

	BoxMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMeshComponent"));
	BoxMeshComponent->SetupAttachment(RootComponent);
	CylinderMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CylinderMeshComponent"));
	CylinderMeshComponent->SetupAttachment(BoxMeshComponent);

	EnemyNotifierBox = CreateDefaultSubobject<UBoxComponent>(TEXT("EnemyNotifierBox"));
	EnemyNotifierBox->SetBoxExtent(FVector(100.f, 100.f, 100.f));
	EnemyNotifierBox->SetCollisionProfileName(TEXT("EnemyNotifier"));
	EnemyNotifierBox->SetupAttachment(RootComponent);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 450.0f;
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	TurretCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TurretCamera"));
	TurretCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TurretCamera->bUsePawnControlRotation = false;

	HealthComponent = CreateDefaultSubobject<UHealthComponentBase>(TEXT("HealthComponent"));
	LifeSpanAfterDeath = 3.0f;
	AbilityPowerLevel = 0.0f;

	// Take control of the default player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ATurretHead::BeginPlay()
{
	Super::BeginPlay();
	OnPlayerScored.AddDynamic(this, &ATurretHead::UpdateScore);
	StartAbilityRegen();
}

void ATurretHead::MouseYaw(float Value)
{
	if (Value == 0.f) return;
	APlayerController* OwnPlayerController = Cast<APlayerController>(Controller);
	OwnPlayerController->AddYawInput(Value);
}

void ATurretHead::MousePitch(float Value)
{
	if (Value != 0.f && Controller && Controller->IsLocalPlayerController())
	{
		AWaveGamePlayerController* const APC = Cast<AWaveGamePlayerController>(Controller);
		APC->AddPitchInput(Value);

		// make controller rotation and character rotation similar in order to aim properly
		FRotator ControllerRotation = APC->GetControlRotation();
		RootComponent->SetWorldRotation(FRotator(ControllerRotation.Pitch, ControllerRotation.Yaw, 0.0f));
	}
}

FVector ATurretHead::GetNormalizedVector(FVector Input)
{
	float Magnitude = FMath::Sqrt(FMath::Pow(Input.X, 2) + FMath::Pow(Input.Y, 2) + FMath::Pow(Input.Y, 2));
	//UE_LOG(LogTemp, Warning, TEXT("ATurretHead::GetNormalizedVector - Magnitude %f"), Magnitude);

	return FVector(Input.X / Magnitude, Input.Y / Magnitude, Input.Z / Magnitude);
}

void ATurretHead::CalculateTurretPitchVectors()
{
	// logging location of the object
	//FVector ActorLocation = GetActorLocation();
	//FVector AxisVector = FVector(ActorLocation.X, 0.0f, 0.0f);
	//UE_LOG(LogTemp, Error, TEXT("ATurretHead::BeginPlay - Actor Location %f, %f, %f"), ActorLocation.X, ActorLocation.Y, ActorLocation.Z);


	//FVector NormalizedVector = GetNormalizedVector(GetActorForwardVector());
	//UE_LOG(LogTemp, Warning, TEXT("ATurretHead::BeginPlay - NormalizedVector %f, %f, %f"), NormalizedVector.X, NormalizedVector.Y, NormalizedVector.Z);

	//FVector ScaledVector = NormalizedVector * 500;
	//UE_LOG(LogTemp, Warning, TEXT("ATurretHead::BeginPlay - ScaledVector %f, %f, %f"), ScaledVector.X, ScaledVector.Y, ScaledVector.Z);


	//FVector ScaledVecotrHeight = ScaledVector + FVector(ScaledVector.X, ScaledVector.Y, ScaledVector.Z - 150.0f);

	//DrawDebugLine(GetWorld(), ActorLocation, ActorLocation - ScaledVector, FColor::Emerald, false, 1, 0, 4);
	//DrawDebugLine(GetWorld(), ActorLocation, ActorLocation - ScaledVecotrHeight, FColor::Red, false, 1, 0, 4);
}

FRotator ATurretHead::GetVectorForTurretDirection()
{
	FVector StartingPosition = CylinderMeshComponent->GetSocketLocation("ShootingPoint");
	//UE_LOG(LogTemp, Warning, TEXT("ShootingPoint %f, %f, %f"), StartingPosition.X, StartingPosition.Y, StartingPosition.Z);

	FVector MiddlePosition = CylinderMeshComponent->GetSocketLocation("MiddlePoint");
	//UE_LOG(LogTemp, Warning, TEXT("MiddlePosition %f, %f, %f"), MiddlePosition.X, MiddlePosition.Y, MiddlePosition.Z);

	FVector DirectionVectorFromOrigin = StartingPosition - MiddlePosition;
	//UE_LOG(LogTemp, Error, TEXT("DirectionVectorFromOrigin %f, %f, %f"), DirectionVectorFromOrigin.X, DirectionVectorFromOrigin.Y, DirectionVectorFromOrigin.Z);

	//FVector EndPosition = FVector(StartingPosition.X + 100.0f, StartingPosition.Y, StartingPosition.Z);
	FVector EndPosition = MiddlePosition + ( DirectionVectorFromOrigin * 20 );
	DrawDebugLine(GetWorld(), MiddlePosition, EndPosition, FColor::Cyan, false, 1, 0, 4);

	FVector EndVector = DirectionVectorFromOrigin;
	return EndVector.Rotation();
}

void ATurretHead::Fire()
{

	//https://forums.unrealengine.com/t/component-has-local-offset-when-spawn-located-at-world-0-0-0/93834/2

	if (ProjectileRef != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World == nullptr) return;

		const FVector SpawnLocation = CylinderMeshComponent->GetSocketLocation("ShootingPoint");
		//DrawDebugSphere(GetWorld(), SpawnLocation, 5, 10, FColor(181, 0, 0), true, -1, 0, 2);
		const FRotator SpawnRotation = GetVectorForTurretDirection();
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ABasicProjectile* SpawnedProjectile = World->SpawnActor<ABasicProjectile>(ProjectileRef, SpawnLocation, SpawnRotation, ActorSpawnParams);
		SpawnedProjectile->ProjectileOwner = this;
		
		if (FireSound != nullptr)
		{
			//UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}
	}
}

// Called every frame
void ATurretHead::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATurretHead::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Turn", this, &ATurretHead::MouseYaw);
	PlayerInputComponent->BindAxis("LookUp", this, &ATurretHead::MousePitch);

	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &ATurretHead::Fire);
	PlayerInputComponent->BindAction("ForcePush", IE_Pressed, this, &ATurretHead::SpawnForcePush);
}

void ATurretHead::Die()
{
	AController* PCtrl = GetController();
	if (PCtrl)
	{
		TurretStatus = TurretState::DEAD;
		this->SetLifeSpan(LifeSpanAfterDeath);
		PCtrl->UnPossess();
	}
	OnPlayerDied.Broadcast();
}

TurretState ATurretHead::GetTurretStatus()
{
	return TurretStatus;
}

void ATurretHead::SetTurretStatus(TurretState State)
{
	TurretStatus = State;
}

void ATurretHead::Reset()
{
	if (HealthComponent)
	{
		HealthComponent->ResetHealth();
	}
}

void ATurretHead::UpdateScore(bool Reset)
{
	if (Reset)
	{
		Score = 0.0f;
	}
	else
	{
		Score += 10.0f;
	}
	UE_LOG(LogTemp, Error, TEXT("UPDATED SCORE %f"), Score);
}

float ATurretHead::GetScore()
{
	return Score;
}

void ATurretHead::ResetPlayerScore()
{
	Score = 0.0f;
}

void ATurretHead::SpawnForcePush()
{
	if (AbilityPowerLevel != 100.f) return;

	if (IsValid(PushActor))
	{
		FActorSpawnParameters SpawnParams;
		AForcePush* SpawnedPushActor = GetWorld()->SpawnActor<AForcePush>(PushActor, FVector(0.0f, 0.0f, 20.0f), FRotator(0.0f, 0.0f, 0.0f), SpawnParams);

		/*if (SpawnedPushActor != nullptr)
		{
			UE_LOG(LogTemp, Log, TEXT("PUSH ACTOR SPAWNED"));
		}*/
	}
	AbilityPowerLevel = 0.0f;
	OnAbilityAmountUpdate.Broadcast(AbilityPowerLevel);
	StartAbilityRegen();
}

void ATurretHead::RestoreAbility()
{
	AbilityPowerLevel += 10.f;
	OnAbilityAmountUpdate.Broadcast(AbilityPowerLevel);
	if (AbilityPowerLevel == 100.f)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_RestoreAbility);
	}
}

void ATurretHead::StartAbilityRegen()
{
	GetWorldTimerManager().SetTimer(TimerHandle_RestoreAbility, this, &ATurretHead::RestoreAbility, 1.0f, true, 1.0f);
}
