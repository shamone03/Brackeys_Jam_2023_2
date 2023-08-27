// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoidCageSpawner.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class ABoid;
class AFlockManager;

UCLASS()
class BRACKEYS_JAM_2023_2_API ABoidCageSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoidCageSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere, Category = "Boid|Spawn")
	float _cooldown;
	UPROPERTY(EditAnywhere, Category = "Boid|Spawn")
	float _currentCount;
	UPROPERTY(EditAnywhere, Category = "Boid|Spawn")
	int32 _spawnAmount;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boid|Components")
	UBoxComponent* _cageCollision;

	UPROPERTY(VisibleAnywhere, Category = "Boid|Components")
	UStaticMeshComponent* _cageMesh;

protected:
	UFUNCTION()
	void OnCageOverlapEnd(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex);

public:
	UPROPERTY(EditAnywhere, Category = "Boid|Spawn", meta = (ClampMin = "0", ClampMax = "500"))
	int32 _numBoidsToSpawn;

	UPROPERTY(EditAnywhere, Category = "Boid|Spawn")
	TSubclassOf<ABoid> _boidType;

	void SpawnBoids(int numBoids);

	UPROPERTY(EditAnywhere, Category = "Boid|Spawn")
	AFlockManager* _assignedFlockManager;
};
