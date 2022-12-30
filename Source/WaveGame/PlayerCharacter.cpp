// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/* init CameraBoom */
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 450.0f; // distance that the camera sits behind the player
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	PlayerFollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerFollowCamera"));
	PlayerFollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // attaching camera to end of the boom by specifying boom end socket name
	PlayerFollowCamera->bUsePawnControlRotation = false; // camera only rotates relative to the boom

	/*
		these are by default unset/ false
		but what these do is disabling object rotation according to the control rotation. Enable these for better understanding
	*/
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 450.0f, 0.0f); // ...at this rotation rate
	//GetCharacterMovement()->JumpZVelocity = 600.f;
	//GetCharacterMovement()->AirControl = 0.2f;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/*
	some reference regarding rotation - https://unrealcpp.com/rotating-actor/
	rotation matrix in math - https://www.youtube.com/watch?v=OYuoPTRVzxY
*/
void APlayerCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		FRotator Rotation = Controller->GetControlRotation();
		//UE_LOG(LogTemp, Warning, TEXT("Current Rotation %f - %f - %f"), Rotation.Vector().X, Rotation.Vector().Y, Rotation.Vector().Z);

		FRotator NewYawRotation(0, Rotation.Yaw, 0); // this confuses me
		//FRotator NewYawRotation = FRotator(0, Rotation.Yaw, 0);
		// FRotationMatrix(NewYawRotation) - Giving a rotation and taking back a vector. Because we need a vector to decide the direction that we need to go
		// .GetUnitAxis(EAxis::X) - This means give me the direction difference from the global X axis. Because our forward is X and it is normalized
		FVector Direction = FRotationMatrix(NewYawRotation).GetUnitAxis(EAxis::X);
		UE_LOG(LogTemp, Warning, TEXT("Current Direction %f - %f - %f"), Direction.X, Direction.Y, Direction.Z);

		// another way to get forward direction of an object is Rotation.Vector()
		// which is similar to blueprint method called RotationFromXVector

		AddMovementInput(Direction, Value);


		//AddMovementInput(GetActorForwardVector(), Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		FRotator Rotation = Controller->GetControlRotation(); // getting forward rotation/ vector relative to world direction
		//UE_LOG(LogTemp, Warning, TEXT("Current Rotation %f - %f - %f"), Rotation.Vector().X, Rotation.Vector().Y, Rotation.Vector().Z);

		FRotator NewYawRotation = FRotator(0, Rotation.Yaw, 0);
		FVector Direction = FRotationMatrix(NewYawRotation).GetUnitAxis(EAxis::Y);
		UE_LOG(LogTemp, Warning, TEXT("Current Direction %f - %f - %f"), Direction.X, Direction.Y, Direction.Z);

		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::MousePitch(float Value)
{
	float MaxAnglePitch = 30.f;
	APlayerController* PC = Cast<APlayerController>(Controller);
	FRotator WorldControlRotation = GetControlRotation().GetNormalized();

	UE_LOG(LogTemp, Warning, TEXT("World Pitch - %f"), GetControlRotation().Pitch);

	const float ScaledInput = Value * PC->InputPitchScale;
	const float PitchSum = WorldControlRotation.Pitch + ScaledInput;
	if (PitchSum > -MaxAnglePitch && PitchSum < MaxAnglePitch)
	{
		PC->RotationInput.Pitch += ScaledInput;
	}
	else if (
		WorldControlRotation.Pitch > -MaxAnglePitch &&
		WorldControlRotation.Pitch < MaxAnglePitch
		)
	{
		const float InputRemainder = FMath::Sign(ScaledInput) * (MaxAnglePitch - FMath::Abs(WorldControlRotation.Pitch));
		PC->RotationInput.Pitch += InputRemainder;
	}
}

void APlayerCharacter::MouseYaw(float Value)
{
	if (Value == 0.0f) return;
	APlayerController* OwnPlayerController = Cast<APlayerController>(Controller);
	FRotator WorldControlRotation = GetControlRotation();

	// getting local rotation
	FRotator LocalControlRotation = FRotator(RootComponent->GetComponentTransform().GetRotation() * WorldControlRotation.Quaternion().Inverse());

	const float ScaledInput = Value * OwnPlayerController->InputYawScale;
	const float YawSum = LocalControlRotation.Yaw + ScaledInput * -1.f; // why we have to multiply it by -1?
	if (YawSum > -90.f && YawSum < 90.f)
	{
		OwnPlayerController->AddYawInput(Value);
	}
	else if (LocalControlRotation.Yaw > -90.f && LocalControlRotation.Yaw < 90.f)
	{
		const float InputRemainder = FMath::Sign(Value) * FMath::Abs((90.f - FMath::Abs(LocalControlRotation.Yaw)) / OwnPlayerController->InputYawScale);
		
		/*UE_LOG(LogTemp, Warning, TEXT("FMath::Abs(LocalControlRotation.Yaw) - %f"), FMath::Abs(LocalControlRotation.Yaw));
		UE_LOG(LogTemp, Warning, TEXT("(90.f - FMath::Abs(LocalControlRotation.Yaw)) - %f"), (90.f - FMath::Abs(LocalControlRotation.Yaw)));
		UE_LOG(LogTemp, Warning, TEXT("OwnPlayerController->InputYawScale - %f"), OwnPlayerController->InputYawScale);
		UE_LOG(LogTemp, Warning, TEXT("FMath::Abs((90.f - FMath::Abs(LocalControlRotation.Yaw)) / OwnPlayerController->InputYawScale) - %f"), FMath::Abs((90.f - FMath::Abs(LocalControlRotation.Yaw)) / OwnPlayerController->InputYawScale));
		UE_LOG(LogTemp, Warning, TEXT("FMath::Sign(Value) - %f"), FMath::Sign(Value));

		UE_LOG(LogTemp, Error, TEXT("InputRemainder %f"), InputRemainder);
		UE_LOG(LogTemp, Error, TEXT("------------------------------------------------"));*/
		
		OwnPlayerController->AddYawInput(InputRemainder);
	}

	///*UE_LOG(LogTemp, Warning, TEXT("MouseYaw - LocalRotation Before - Yaw %f"), LocalControlRotation.Yaw);
	//UE_LOG(LogTemp, Error, TEXT("MouseYaw - WorldRotation Before - Yaw %f"), WorldControlRotation.Yaw);*/
	//
	//if (OwnPlayerController)
	//{
	//	if (LocalControlRotation.Yaw >= -90.0f && LocalControlRotation.Yaw <= 90.0f)
	//	{
	//		OwnPlayerController->AddYawInput(Value);
	//	}
	//	else
	//	{
	//		FRotator ControllerCurrentRotation = LocalControlRotation;
	//		float ControlledYawValue = FMath::Clamp(ControllerCurrentRotation.Yaw, -85.0f, 85.0f);
	//		//UE_LOG(LogTemp, Warning, TEXT("MouseYaw - Clamped Value - Yaw %f"), ControlledYawValue);
	//		
	//		FRotator NewControllerRotation = FRotator(0.0f, ControlledYawValue, 0.0f);
	//		FRotator TestRotator = FRotator(RootComponent->GetComponentTransform().GetRotation() * NewControllerRotation.Quaternion().Inverse());

	//		/*UE_LOG(LogTemp, Error, TEXT("MouseYaw - NewControllerRotation %f, %f. %f"), TestRotator.Pitch, TestRotator.Yaw, TestRotator.Roll);*/
	//		
	//		OwnPlayerController->SetControlRotation(FRotator(WorldControlRotation.Pitch, TestRotator.Yaw, WorldControlRotation.Roll));
	//		//OwnPlayerController->SetControlRotation(TestRotator);

	//	}
	//}
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::MouseYaw);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::MousePitch);
}

