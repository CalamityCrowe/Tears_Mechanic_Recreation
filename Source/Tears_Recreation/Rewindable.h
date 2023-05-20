// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <utility>   
#include "Rewindable.generated.h"

using std::pair; 

UCLASS()
class TEARS_RECREATION_API ARewindable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARewindable();

	UFUNCTION(BlueprintCallable)
		void RewindObject(float lerpIntensity);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	TQueue<pair<FVector, FRotator>> m_RewindPositions;
	bool m_isRewinding;

};
