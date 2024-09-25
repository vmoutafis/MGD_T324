// Fill out your copyright notice in the Description page of Project Settings.


#include "MGCharacter.h"

#include "Kismet/KismetMathLibrary.h"

// Sets default values
AMGCharacter::AMGCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AMGCharacter::PMoveCharacter(FVector2D axis)
{
	// create a local rotation variable
	FRotator controlRot;

	// set the yaw to the same as the controllers yaw
	controlRot.Yaw = GetControlRotation().Yaw;

	// move the character forward and back
	AddMovementInput(UKismetMathLibrary::GetForwardVector(controlRot), axis.Y);

	// move the character right and left
	AddMovementInput(UKismetMathLibrary::GetRightVector(controlRot), axis.X);
}

void AMGCharacter::ToggleTrigger(const bool pullOrRelease)
{
	if (HasAuthority() && IsLocallyControlled())
	{
		Multi_OnTriggerChanged(pullOrRelease);
		return;
	}

	if (!HasAuthority() && IsLocallyControlled())
		Server_OnTriggerChanged(pullOrRelease);
}

void AMGCharacter::Multi_OnTriggerChanged_Implementation(const bool pullOrRelease)
{
	bIsHoldingTrigger = pullOrRelease;
	OnTriggerChanged(pullOrRelease);
}

void AMGCharacter::Server_OnTriggerChanged_Implementation(const bool pullOrRelease)
{
	Multi_OnTriggerChanged(pullOrRelease);
}

