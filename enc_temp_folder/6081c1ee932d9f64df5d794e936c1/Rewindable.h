// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <utility>   
#include "Containers/CircularQueue.h"
#include "Components/SplineComponent.h"
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
	UFUNCTION(BlueprintCallable)
		void SetRewind(bool b)
	{
		m_isRewinding = b;
		StartMove = true;
	}
	bool GetRewind() { return m_isRewinding; }
	UFUNCTION(BlueprintPure)
		int rewindCount() { return m_RewindPositions->Num(); }
	UFUNCTION(BlueprintCallable)
		void GrabPosition();





protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	TArray<pair<FVector, FRotator>>* m_RewindPositions;
	bool m_isRewinding;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
		class UStaticMeshComponent* StaticMesh;
	pair<FVector, FRotator> nextPosition;

	bool StartMove = true;
};
