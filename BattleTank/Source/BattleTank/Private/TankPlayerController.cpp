// Fill out your copyright notice in the Description page of Project Settings.


#include "TankPlayerController.h"
#include "Tank.h"



void ATankPlayerController::BeginPlay()
{
	Super::BeginPlay();

	auto ControlledTank = GetControlledTank();
	if (!ControlledTank)
	{
		UE_LOG(LogTemp, Warning, TEXT("Playercontroller not possesing a tank"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Playercontroller possesing: %s"), *(ControlledTank->GetName()));
	}
}

// Called every frame
void ATankPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AimTowardsCrosshair();
}


ATank* ATankPlayerController::GetControlledTank() const
{
	return Cast<ATank>(GetPawn());
}


void ATankPlayerController::AimTowardsCrosshair()
{
	if (!GetControlledTank()) { return; }


	FVector HitLocation; // Out parameter
	if (GetSightRayHitLocation(HitLocation)) // Has "side-effect", is going to line trace
	{
		// UE_LOG(LogTemp, Warning, TEXT("HitLocation: %s"), *HitLocation.ToString());
		
		// TODO Tell controlled tank to aim at this point

		GetControlledTank()->AimAt(HitLocation);
	}
}

// Get world location if linetrace through crosshair, true if hits the landscape
bool ATankPlayerController::GetSightRayHitLocation(FVector& HitLocation) const
{

	// Find the crosshair position
	int32 ViewportSizeX, ViewportSizeY;
	GetViewportSize(ViewportSizeX, ViewportSizeY);
	auto ScreenLocation = FVector2D(ViewportSizeX * CrosshairXLocation, ViewportSizeY * CrosshairYLocation);

	FVector LookDirection;
	// "De-project" the screen position of the crosshair to a world direction
	if (GetLookDirection(ScreenLocation, LookDirection))
	{
		// Line-trace along that LookDirection, and see what we hit (up to max range)
		// UE_LOG(LogTemp, Warning, TEXT("Look direction: %s"), *LookDirection.ToString());
		return GetLookVectorHitLocation(LookDirection, HitLocation);
	}
	
	return false;
}



bool ATankPlayerController::GetLookVectorHitLocation(FVector& LookDirection, FVector& HitLocation) const
{
	FHitResult HitResult;
	auto StartLoacation = PlayerCameraManager->GetCameraLocation();
	auto EndLocation = StartLoacation + (LookDirection * LineTraceRange);
	if (GetWorld()->LineTraceSingleByChannel(
			HitResult,
			StartLoacation,
			EndLocation,
			ECollisionChannel::ECC_Camera)
		)
	{
		HitLocation = HitResult.Location;
		return true;
	}
	HitLocation = FVector(0);
	return false; // Line trase didnt succed
}
 

bool ATankPlayerController::GetLookDirection(FVector2D ScreenLocation, FVector& LookDirection) const
{
	FVector CameraWorldLocation; // To be discarded
	return DeprojectScreenPositionToWorld(
		ScreenLocation.X,
		ScreenLocation.Y,
		CameraWorldLocation,
		LookDirection
	);

}