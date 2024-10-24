// Fill out your copyright notice in the Description page of Project Settings.


#include "MGPickup.h"

#include "MGCharacter.h"

// Sets default values
AMGPickup::AMGPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// make sure the pickup replicates
	SetReplicates(true);

	// create the component
	PickupTrigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));
	// set the component to be the root of the class
	SetRootComponent(PickupTrigger);

	// set the default size of the collision
	PickupTrigger->SetCapsuleHalfHeight(100.0f);
	PickupTrigger->SetCapsuleRadius(100.0f);

	// add a static mesh component to the actor
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	// attach the mesh component to the root component, this is the collision since we set it earlier
	MeshComponent->SetupAttachment(RootComponent);
	// move the mesh component up by default
	MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	// set the collisions of the mesh component to none so we can walk through it
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// this will be the default time for the class but can be changed in the blueprint
	ResetTime = 10.0f;
}

void AMGPickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!HasAuthority())
		return;
	
	if (OtherActor->GetClass()->IsChildOf(AMGCharacter::StaticClass()))
	{
		ActivatePickup(OtherActor);
	}
}

void AMGPickup::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		PickupTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AMGPickup::ActivatePickup(AActor* PickupActor)
{
	BP_OnPickupActivated(PickupActor);

	DeactivatePickup();
}

void AMGPickup::ReactivatePickup_Implementation()
{
	PickupTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	MeshComponent->SetVisibility(true, true);
}

void AMGPickup::DeactivatePickup_Implementation()
{
	PickupTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MeshComponent->SetVisibility(false, true);

	if (ResetTime > 0.0f && HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(TH_ResetTimer, this, &AMGPickup::ReactivatePickup, ResetTime);
	}
}


