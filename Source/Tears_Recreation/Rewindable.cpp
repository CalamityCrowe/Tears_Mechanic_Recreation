// Fill out your copyright notice in the Description page of Project Settings.


#include "Rewindable.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ARewindable::ARewindable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = StaticMesh;
	m_RewindPositions = new TArray<pair<FVector, FRotator>>();

}

void ARewindable::RewindObject(float lerpIntensity)
{
	if (m_isRewinding == true)
	{
		if (m_RewindPositions->IsEmpty())
		{
			m_isRewinding = false;
			
		}
		else
		{
			if (abs(GetActorLocation().Length() - m_RewindPositions->Top().first.Length()) > 1)
			{
				nextPosition = m_RewindPositions->Top();
			}
			else
			{
				m_RewindPositions->Pop();
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Emptying")));
			}
			FVector tempPos = UKismetMathLibrary::VLerp(GetActorLocation(), nextPosition.first, lerpIntensity);
			FRotator tempRot = UKismetMathLibrary::RLerp(GetActorRotation(), nextPosition.second, lerpIntensity, false);
			SetActorLocationAndRotation(tempPos, tempRot);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("%d"), m_RewindPositions->Num()));
		}
	}
}

void ARewindable::GrabPosition()
{
	if (m_isRewinding == false)
	{
		if (m_RewindPositions->Num() < 30)
		{
			m_RewindPositions->Push(pair<FVector, FRotator>(GetActorLocation(), GetActorRotation()));
		}
		else
		{
			m_RewindPositions->RemoveAt(0); 
			m_RewindPositions->Push(pair<FVector, FRotator>(GetActorLocation(), GetActorRotation()));
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("AAAAAAAAAA")));

		}
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("%d"), m_RewindPositions->Num()));

	}
}

// Called when the game starts or when spawned
void ARewindable::BeginPlay()
{
	Super::BeginPlay();
	GrabPosition();
}

void ARewindable::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	m_RewindPositions->Empty();
}

// Called every frame
void ARewindable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (m_isRewinding)
	{
		StaticMesh->SetSimulatePhysics(false);
	}
	else
	{
		StaticMesh->SetSimulatePhysics(true);
	}

}

