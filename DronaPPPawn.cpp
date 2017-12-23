// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "DronaPPPawn.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"

ADronaPPPawn::ADronaPPPawn()
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Flying/Meshes/UFO.UFO"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create static mesh component
	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh0"));
	PlaneMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());	// Set static mesh
	RootComponent = PlaneMesh;

	//Pune mesh-ul dronei pentru al reusit sa il rotim fara sa schimbam axele
	Drona = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Drona"));
	Drona->SetupAttachment(RootComponent);

	//Atasam mesh-ul elicelor dronei
	Elice1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Elice1"));
	Elice1->SetupAttachment(RootComponent);

	Elice2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Elice2"));
	Elice2->SetupAttachment(RootComponent);

	Elice3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Elice3"));
	Elice3->SetupAttachment(RootComponent);

	Elice4 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Elice4"));
	Elice4->SetupAttachment(RootComponent);


	// Create a spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->SetupAttachment(RootComponent);	// Attach SpringArm to RootComponent
	SpringArm->TargetArmLength = 160.0f; // The camera follows at this distance behind the character	
	SpringArm->SocketOffset = FVector(0.f,0.f,60.f);
	SpringArm->bEnableCameraLag = false;	// Do not allow camera to lag
	SpringArm->CameraLagSpeed = 15.f;

	// Create camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);	// Attach the camera
	Camera->bUsePawnControlRotation = false; // Don't rotate camera with controller

	// Set handling parameters
	Acceleration = 5000.0f;
	TurnSpeed = 500.0f;
	MaxSpeed = 4000.0f;
	MinSpeed = 0.0f;

	RotationRate = FRotator(0.0f, 180.f, 0.0f);
	
}

void ADronaPPPawn::Tick(float DeltaSeconds)
{
	const FVector LocalMove = FVector(CurrentXSpeed * DeltaSeconds, CurrentYSpeed * DeltaSeconds, CurrentZSpeed * DeltaSeconds);

	// Move plan forwards (with sweep so we stop when we collide with things)
	AddActorLocalOffset(LocalMove, true);

	// Calculate change in rotation this frame
	FRotator DeltaRotation(0,0,0);
	DeltaRotation.Pitch = CurrentPitchSpeed * DeltaSeconds;
	DeltaRotation.Yaw = CurrentYawSpeed * DeltaSeconds;
	DeltaRotation.Roll = CurrentRollSpeed * DeltaSeconds;

	// Rotate plane
	AddActorLocalRotation(DeltaRotation);
	Elice1->AddLocalRotation(-1*RotationRate * DeltaSeconds);
	Elice2->AddLocalRotation(RotationRate * DeltaSeconds);
	Elice3->AddLocalRotation(-1*RotationRate * DeltaSeconds);
	Elice4->AddLocalRotation(RotationRate * DeltaSeconds);

	// Call any parent class Tick implementation
	Super::Tick(DeltaSeconds);
}

void ADronaPPPawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// Deflect along the surface when we collide.
	FRotator CurrentRotation = GetActorRotation();
	SetActorRotation(FQuat::Slerp(CurrentRotation.Quaternion(), HitNormal.ToOrientationQuat(), 0.025f));
}


void ADronaPPPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    // Check if PlayerInputComponent is valid (not NULL)
	check(PlayerInputComponent);

	// Bind our control axis' to callback functions
	PlayerInputComponent->BindAxis("MoveX", this, &ADronaPPPawn::MoveXInput);
	PlayerInputComponent->BindAxis("MoveY", this, &ADronaPPPawn::MoveYInput);
	PlayerInputComponent->BindAxis("MoveZ", this, &ADronaPPPawn::MoveZInput);
	PlayerInputComponent->BindAxis("ZYaw", this, &ADronaPPPawn::ZYawInput);
}

void ADronaPPPawn::MoveXInput(float Val)
{
	bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);
	FRotator RotationRate;
	FRotator CurrentRotation;
	float CurrentAcc;
	if (bHasInput)
	{
		FRotator RotationRate = { -2.0f * FMath::Sign(Val), 0.0f, 0.0f };

		if (FMath::Abs(Drona->GetComponentRotation().Pitch) < 20.0f)
		{
			Drona->AddLocalRotation(RotationRate);
			Elice1->AddLocalRotation(RotationRate);
			Elice2->AddLocalRotation(RotationRate);
			Elice3->AddLocalRotation(RotationRate);
			Elice4->AddLocalRotation(RotationRate);
		}

		CurrentAcc = Val * Acceleration;

	}
	else
	{
		FRotator RotationRate = { -1.0f * FMath::Sign(Drona->GetComponentRotation().Pitch), 0.0f, 0.0f };

		if (Drona->GetComponentRotation().Pitch != 0)
		{
			Drona->AddLocalRotation(RotationRate);
			Elice1->AddLocalRotation(RotationRate);
			Elice2->AddLocalRotation(RotationRate);
			Elice3->AddLocalRotation(RotationRate);
			Elice4->AddLocalRotation(RotationRate);
		}

		CurrentAcc = -CurrentXSpeed;

	}
	FRotator MaxRotation;
	float NewXSpeed = CurrentXSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);
	FRotator NewRotation = CurrentRotation + (GetWorld()->GetDeltaSeconds() * RotationRate);

	CurrentXSpeed = FMath::Clamp(NewXSpeed, -MaxSpeed, MaxSpeed);
	Drona->SetRelativeRotation({ FMath::Clamp(NewRotation.Pitch, -MaxRotation.Pitch, MaxRotation.Pitch), 0.0f, 0.0f });
	Elice1->SetRelativeRotation({ FMath::Clamp(NewRotation.Pitch, -MaxRotation.Pitch, MaxRotation.Pitch), 0.0f, 0.0f });
	Elice2->SetRelativeRotation({ FMath::Clamp(NewRotation.Pitch, -MaxRotation.Pitch, MaxRotation.Pitch), 0.0f, 0.0f });
	Elice3->SetRelativeRotation({ FMath::Clamp(NewRotation.Pitch, -MaxRotation.Pitch, MaxRotation.Pitch), 0.0f, 0.0f });
	Elice4->SetRelativeRotation({ FMath::Clamp(NewRotation.Pitch, -MaxRotation.Pitch, MaxRotation.Pitch), 0.0f, 0.0f });


}

void ADronaPPPawn::MoveYInput(float Val)
{
	bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);
	FRotator RotationRate;
	FRotator CurrentRotation;
	float CurrentAcc;
	if (bHasInput)
	{
		 FRotator RotationRate = { 0.0f, 0.0f, -2.0f * FMath::Sign(Val) };

		 if (FMath::Abs(Drona->GetComponentRotation().Roll) < 20.0f)
		 {
			 Drona->AddLocalRotation(-1 * RotationRate);
			 Elice1->AddLocalRotation(-1 * RotationRate);
			 Elice2->AddLocalRotation(-1 * RotationRate);
			 Elice3->AddLocalRotation(-1 * RotationRate);
			 Elice4->AddLocalRotation(-1 * RotationRate);
		 }	

			
		CurrentAcc = Val * Acceleration;

	}
	else
	{
		FRotator RotationRate = { 0.0f, 0.0f, -1.0f * FMath::Sign(Drona->GetComponentRotation().Roll) };
		if (Drona->GetComponentRotation().Roll != 0)
		{
			Drona->AddLocalRotation(RotationRate);
			Elice1->AddLocalRotation(RotationRate);
			Elice2->AddLocalRotation(RotationRate);
			Elice3->AddLocalRotation(RotationRate);
			Elice4->AddLocalRotation(RotationRate);
		}
		

		CurrentAcc = -CurrentYSpeed;

	}

	FRotator MaxRotation;
	float NewYSpeed = CurrentYSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);
	FRotator NewRotation = CurrentRotation + (GetWorld()->GetDeltaSeconds() * RotationRate);

	CurrentYSpeed = FMath::Clamp(NewYSpeed, -MaxSpeed, MaxSpeed);
	Drona->SetRelativeRotation({ 0.0f, 0.0f, FMath::Clamp(NewRotation.Roll, -MaxRotation.Roll, MaxRotation.Roll) });
	Elice1->SetRelativeRotation({ 0.0f, 0.0f, FMath::Clamp(NewRotation.Roll, -MaxRotation.Roll, MaxRotation.Roll) });
	Elice2->SetRelativeRotation({ 0.0f, 0.0f, FMath::Clamp(NewRotation.Roll, -MaxRotation.Roll, MaxRotation.Roll) });
	Elice3->SetRelativeRotation({ 0.0f, 0.0f, FMath::Clamp(NewRotation.Roll, -MaxRotation.Roll, MaxRotation.Roll) });
	Elice4->SetRelativeRotation({ 0.0f, 0.0f, FMath::Clamp(NewRotation.Roll, -MaxRotation.Roll, MaxRotation.Roll) });

}

void ADronaPPPawn::MoveZInput(float Val)
{
	bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);

	float CurrentAcc;

	if (bHasInput)
		CurrentAcc = Val * Acceleration;
	else
		CurrentAcc = -CurrentZSpeed;

	float NewZSpeed = CurrentZSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);

	CurrentZSpeed = FMath::Clamp(NewZSpeed, -MaxSpeed, MaxSpeed);
}


void ADronaPPPawn::ZYawInput(float Val)
{
	// Target yaw speed is based on input
	float TargetYawSpeed = (Val * TurnSpeed);

	// Smoothly interpolate to target yaw speed
	CurrentYawSpeed = FMath::FInterpTo(CurrentYawSpeed, TargetYawSpeed, GetWorld()->GetDeltaSeconds(), 2.f);


}
