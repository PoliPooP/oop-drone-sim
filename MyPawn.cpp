// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPawn.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Components/SceneComponent.h"
#include "Runtime/Engine/Classes/Components/InputComponent.h"

// Sets default values
AMyPawn::AMyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set this pawn to be controlled by the lowest-numbered player
	AutoPossessPlayer = EAutoReceiveInput::Player0;


	// Create a dummy root component we can attach things to.
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// Create a camera and a visible object
	UCameraComponent* OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("OurCamera"));
	OurVisibleComponent = CreateDefaultSubobject<USceneComponent>(TEXT("OurVisibleComponent"));
	OurMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OurMeshComponent"));

	// Attach our camera and visible object to our root component. Offset and rotate the camera.
	OurCamera->SetupAttachment(RootComponent);
	OurCamera->SetRelativeLocation(FVector(-250.0f, 0.0f, 250.0f));
	OurCamera->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
	OurVisibleComponent->SetupAttachment(RootComponent);
}


// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Handle movement based on our "MoveX" and "MoveY" axes
	{
		if (!CurrentVelocity.IsZero())
		{
			FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
			SetActorLocation(NewLocation);
		}

		UpdateTilt();
	}

	

}

// Called to bind functionality to input
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Respond every frame to the values of our two movement axes, "MoveX" and "MoveY".
	InputComponent->BindAxis("MoveX", this, &AMyPawn::Move_XAxis);
	InputComponent->BindAxis("MoveY", this, &AMyPawn::Move_YAxis);
	InputComponent->BindAxis("MoveZ", this, &AMyPawn::Move_ZAxis);
}
void AMyPawn::Move_XAxis(float AxisValue)
{
	CurrentVelocity.Y = -FMath::Clamp(AxisValue, -1.0f, 1.0f) * Speed;

	// Move at 100 units per second forward or backward
	Rotate_Pitch(AxisValue);
}
void AMyPawn::Move_YAxis(float AxisValue)
{
	// Move at 100 units per second right or left
	CurrentVelocity.X = -FMath::Clamp(AxisValue, -1.0f, 1.0f) * Speed;

	Rotate_Roll(AxisValue);
}
void AMyPawn::Move_ZAxis(float AxisValue)
{
	// Move at 100 units per second right or left
	CurrentVelocity.Z = FMath::Clamp(AxisValue, -1.0f, 1.0f) * Speed;
	Rotate_Yaw(AxisValue);
}
void AMyPawn::Rotate_Yaw(float AxisValue)
{
	currentYaw -= FMath::Clamp(AxisValue, -1.0f, 1.0f) * 0.2f;
}

void AMyPawn::Rotate_Pitch(float AxisValue)
{
	currentPitch -= FMath::Clamp(AxisValue, -1.0f, 1.0f) * 0.2f;
}

void AMyPawn::Rotate_Roll(float AxisValue)
{
	currentRoll -= FMath::Clamp(AxisValue, -1.0f, 1.0f) * 0.2f;
}

void AMyPawn::UpdateTilt()
{
	FRotator currentRotation = GetActorRotation();

	if (FMath::Abs(currentYaw) > 0.01f)
	{
		currentYaw -= 0.15f * FMath::Sign(currentYaw);
	}

	if (FMath::Abs(currentRotation.Pitch - currentPitch) > 0.01f)
	{
		currentPitch -= 0.15f * FMath::Sign(currentPitch);
	}

	if (FMath::Abs(currentRotation.Roll - currentRoll) > 0.01f)
	{
		currentRoll -= 0.15f * FMath::Sign(currentRoll);
	}

	//currentRotation.Yaw -= currentYaw;
	currentRotation.Pitch -= currentPitch;
	currentRotation.Roll -= currentRoll;
	SetActorRotation(currentRotation);
}
