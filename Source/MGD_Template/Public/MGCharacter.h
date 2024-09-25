// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MGCharacter.generated.h"

UCLASS()
class MGD_TEMPLATE_API AMGCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMGCharacter();

	// move the character
	UFUNCTION(BlueprintCallable, Category="Movement")
	void PMoveCharacter(FVector2D axis);

	// this determines if the character is holding the trigger
	UFUNCTION(BlueprintPure, Category="Shooting")
	bool IsHoldingTrigger() const { return bIsHoldingTrigger; }

	// pull or release the trigger
	UFUNCTION(BlueprintCallable, Category="Shooting")
	void ToggleTrigger(const bool pullOrRelease);

	UFUNCTION(Server, Reliable, Category="Shooting")
	void Server_OnTriggerChanged(const bool pullOrRelease);

	UFUNCTION(NetMulticast, Reliable, Category="Shooting")
	void Multi_OnTriggerChanged(const bool pullOrRelease);
	
	UFUNCTION(BlueprintImplementableEvent, Category="Shooting")
	void OnTriggerChanged(const bool pullOrRelease);
	
private:
	// this determines if the character is holding the trigger
	bool bIsHoldingTrigger;
};
