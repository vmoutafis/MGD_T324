// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "MGPickup.generated.h"

UCLASS()
class MGD_TEMPLATE_API AMGPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMGPickup();

	UPROPERTY(EditAnywhere, Category=Components)
	UCapsuleComponent* PickupTrigger;

	UPROPERTY(EditAnywhere, Category=Components)
	UStaticMeshComponent* MeshComponent;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:

	virtual void BeginPlay() override;
	
	// if the pickup has been activted
	virtual void ActivatePickup(AActor* PickupActor);

	UFUNCTION(BlueprintImplementableEvent, Category=Pickup)
	void BP_OnPickupActivated(AActor* PickupActor);

	UFUNCTION(NetMulticast, Reliable, Category=Replication)
	void DeactivatePickup();

	UFUNCTION(NetMulticast, Reliable, Category=Replication)
	void ReactivatePickup();

	UPROPERTY(EditDefaultsOnly, Category=Pickup)
	float ResetTime;

private:

	// the timer for reactivating the pickup
	FTimerHandle TH_ResetTimer;
};
