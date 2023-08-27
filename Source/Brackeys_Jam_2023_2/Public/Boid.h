// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Boid.generated.h"

#define COLLISION_AVOIDANCE				ECC_GameTraceChannel1

class UStaticMeshComponent;
class USphereComponent;
class AFlockManager;

UCLASS()
class BRACKEYS_JAM_2023_2_API ABoid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

// Boid Components **exposed to Blueprints**
public:
	// boid collider component
	UPROPERTY(VisibleAnywhere, Category="Boid|Components")
	USphereComponent* _boidCollider;

	// boid static mesh component
	UPROPERTY(VisibleAnywhere, Category="Boid|Components")
	UStaticMeshComponent* _boidMesh;

	// boid perceptual field
	UPROPERTY(VisibleAnywhere, Category="Boid|Components")
	USphereComponent* _perceptualField;

// Flock manager
protected:
	//flock manager that controls this boid's perception and steering behaviors
	AFlockManager* _flockManager;

public:
	inline AFlockManager* GetFlockManager() { return _flockManager; }

// Movement for Boid
protected:
	FVector _boidVelocity;

	FRotator _currentRotation;

	void UpdateMeshRotation();

public:
	UFUNCTION(BlueprintCallable)
	inline FVector GetBoidVelocity() { return _boidVelocity; };

	// Boid behavior
protected:
	// return separation steering force directed to avoid crowding/collision with local flockmates
	FVector	Separate(TArray<ABoid*> flock);
	// return alignment steering force directed towards the average heading of local flockmates
	FVector Align(TArray<ABoid*> flock);
	// return cohesion steering force directed toward the average position of local flockmates
	FVector GroupUp(TArray<ABoid*> flock);	
	// apply behavioral steering to boid and update movement
	void Steer(float DeltaTime);

	// Obstacle avoidance
protected:
	// checks if boid is on imminent collision course with obstacle
	bool IsObstacleAhead();
	// return obstacle avoidance force steering towards the unobstructed direction
	FVector AvoidObstacle();

	// Target steering
protected:
	// list of target forces
	TArray<FVector> _targetForces;

public:
	// adds target force to stack to be applied
	void AddTargetForce(FVector targetForce);
};
