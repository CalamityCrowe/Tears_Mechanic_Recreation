// Fill out your copyright notice in the Description page of Project Settings.


#include "Rewindable.h"

// Sets default values
ARewindable::ARewindable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh")); 
	RootComponent = StaticMesh; 
	m_RewindPositions = new TCircularQueue<pair<FVector, FRotator>>(29); 

}

void ARewindable::RewindObject(float lerpIntensity)
{
	if (m_isRewinding == true) 
	{
		
	}
}

void ARewindable::GrabPosition()
{
	if (m_isRewinding == false) 
	{
		if (m_RewindPositions->IsFull() == false)
		{
			m_RewindPositions->Enqueue(pair<FVector, FRotator>(GetActorLocation(), GetActorRotation()));
		}
		else
		{
			m_RewindPositions->Dequeue();
			m_RewindPositions->Enqueue(pair<FVector, FRotator>(GetActorLocation(), GetActorRotation()));
		
		}
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("%d"), m_RewindPositions->Count()));

	}
}

// Called when the game starts or when spawned
void ARewindable::BeginPlay()
{
	Super::BeginPlay();
	GrabPosition();
}

// Called every frame
void ARewindable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

