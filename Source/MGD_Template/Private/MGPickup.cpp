// Fill out your copyright notice in the Description page of Project Settings.


#include "MGPickup.h"

// Sets default values
AMGPickup::AMGPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
	
	// create the component
	PickupTrigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));
	// set the component to be the root of the class
	SetRootComponent(PickupTrigger);

	// set the default size of the collision
	PickupTrigger->SetCapsuleHalfHeight(100.0f);
	PickupTrigger->SetCapsuleRadius(100.0f);

	// creating the static mesh component
	// without this, the component won't exist or appear in the blueprint
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	// attach it to the actor by attaching it to a component
	Mesh->SetupAttachment(RootComponent);

	// move the mesh up by default
	Mesh->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));

	// turn the collisions off on the mesh
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// default allowed pickup class at actor
	PickupActorClass = AActor::StaticClass();

	// set the default for the reset timer to 10 seconds
	ResetTime = 10.0f;

	MeshRotateSpeed = 50.0f;
}

void AMGPickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!HasAuthority())
	{
		return;
	}
	
	if (OtherActor->GetClass()->IsChildOf(PickupActorClass))
	{
		ActivatePickup(OtherActor);
	}
}

void AMGPickup::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// spinning the mesh based on rotation speed
	Mesh->AddLocalRotation(FRotator(0.0f, MeshRotateSpeed * DeltaSeconds, 0.0f));
}

void AMGPickup::ActivatePickup(AActor* pickupActor)
{
	BP_OnActivatePickup(pickupActor);

	DeactivatePickup();

	// checking if the reset time is greater than 0
	if (ResetTime > 0.0f)
	{
		// if so, set a timer to reactivate the pickup
		GetWorld()->GetTimerManager().SetTimer(TH_ReactivateTimer, this, &AMGPickup::ReactivatePickup, ResetTime);
	}
}

void AMGPickup::ReactivatePickup_Implementation()
{
	// is this the server version
	if (HasAuthority())
	{
		PickupTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	// regardless of server or client reveal the mesh
	Mesh->SetVisibility(true, true);
}

void AMGPickup::DeactivatePickup_Implementation()
{
	// is this the server version
	if (HasAuthority())
	{
		// if so, disable the collision so it can't be collected
		PickupTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// regardless of server or client hide the mesh
	Mesh->SetVisibility(false, true);
}
