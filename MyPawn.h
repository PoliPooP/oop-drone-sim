// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyPawn.generated.h"

UCLASS()
class PROIECT_API AMyPawn : public APawn
{
	GENERATED_BODY()

private:
	float currentYaw = 0.f;
	float currentPitch = 0.f;
	float currentRoll = 0.f;

public:
	// Sets default values for this pawn's properties
	AMyPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
		USceneComponent* OurVisibleComponent;
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* OurMeshComponent;

	UPrimitiveComponent * Force1;
	
	//Input functions
	void Move_XAxis(float AxisValue);
	void Move_YAxis(float AxisValue);
	void Move_ZAxis(float AxisValue);

	void Rotate_Yaw(float AxisValue);
	void Rotate_Pitch(float AxisValue);
	void Rotate_Roll(float AxisValue);

	void UpdateTilt();

	//Input variables
	FVector CurrentVelocity;

	UPROPERTY(EditAnywhere)
		FRotator RotationSpeed;

	UPROPERTY(EditAnywhere)
		float Speed;


	//Forces
};
