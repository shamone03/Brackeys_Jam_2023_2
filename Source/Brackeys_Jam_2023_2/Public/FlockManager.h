// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlockManager.generated.h"

class UBillboardComponent;
class ABoid;

UCLASS()
class BRACKEYS_JAM_2023_2_API AFlockManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFlockManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

// Components
protected:
	UPROPERTY(VisibleAnywhere, Category = "Boid|Components")
	UBillboardComponent* flockManagerBillboard;

// Flock of boids
protected:
	//list of boids in flock
	TArray<ABoid*> boidsInFlock;

public:
	void AddBoidToFlock(ABoid* boid);
	void RemoveBoidFromFlock(ABoid* boid);

// Movement
protected:
	UPROPERTY(EditAnywhere, Category = "Boid|Movement", meta = (ClampMin = "0.0"))
	float maxSpeed;
	UPROPERTY(EditAnywhere, Category = "Boid|Movement", meta = (ClampMin = "0.0"))
	float minSpeed;

public:
	UFUNCTION(BlueprintCallable, Category = "Boid|Movement")
	inline float GetMaxSpeed() { return maxSpeed; };
	UFUNCTION(BlueprintCallable, Category = "Boid|Movement")
	inline float GetMinSpeed() { return minSpeed; };
	UFUNCTION(BlueprintCallable, Category = "Boid|Movement")
	void SetMaxSpeed(float newMaxSpeed);
	UFUNCTION(BlueprintCallable, Category = "Boid|Movement")
	void SetMinSpeed(float newMinSpeed);

// Boid steering
protected:
	//behavioral steering force strengths
	UPROPERTY(EditAnywhere, Category = "Boid|Steering")
	float alignmentStrength;
	UPROPERTY(EditAnywhere, Category = "Boid|Steering")
	float separationStrength;
	UPROPERTY(EditAnywhere, Category = "Boid|Steering")
	float cohesionStrength;
	UPROPERTY(EditAnywhere, Category = "Boid|Steering")
	float avoidanceStrength;

public:
	//steering property getters + setters
	UFUNCTION(BlueprintCallable, Category = "Boid|Steering")
	inline float GetAlignmentStrength() { return alignmentStrength; };
	UFUNCTION(BlueprintCallable, Category = "Boid|Steering")
	inline float GetSeparationStrength() { return separationStrength; };
	UFUNCTION(BlueprintCallable, Category = "Boid|Steering")
	inline float GetCohesionStrength() { return cohesionStrength; };
	UFUNCTION(BlueprintCallable, Category = "Boid|Steering")
	inline float GetAvoidanceStrength() { return avoidanceStrength; };
	UFUNCTION(BlueprintCallable, Category = "Boid|Steering")
	void SetAlignmentStrength(float newAlignmentStrength);
	UFUNCTION(BlueprintCallable, Category = "Boid|Steering")
	void SetSeparationStrength(float newSeparationStrength);
	UFUNCTION(BlueprintCallable, Category = "Boid|Steering")
	void SetCohesionStrength(float newCohesionStrength);
	UFUNCTION(BlueprintCallable, Category = "Boid|Steering")
	void SetAvoidanceStrength(float newAvoidanceStrength);

// Boid perception
protected:
	//determines the field of view of each perception field to determine if a flockmate is sensed (1.0 boid can only sense things directly in front of it, -1 boid can sense in all directions)
	UPROPERTY(EditAnywhere, Category = "Boid|Perception", meta = (ClampMin = "-1.0", ClampMax = "1.0"))
	float separationFOV;
	UPROPERTY(EditAnywhere, Category = "Boid|Perception", meta = (ClampMin = "-1.0", ClampMax = "1.0"))
	float alignmentFOV;
	UPROPERTY(EditAnywhere, Category = "Boid|Perception", meta = (ClampMin = "-1.0", ClampMax = "1.0"))
	float cohesionFOV;

public:
	//getters + setters
	UFUNCTION(BlueprintCallable, Category = "Boid|Perception")
	inline float GetSeparationFOV() { return separationFOV; };
	UFUNCTION(BlueprintCallable, Category = "Boid|Perception")
	inline float GetAlignmentFOV() { return alignmentFOV; };
	UFUNCTION(BlueprintCallable, Category = "Boid|Perception")
	inline float GetCohesionFOV() { return cohesionFOV; };

// Boid avoidance
protected:
	//number of avoidance sensors
	UPROPERTY(EditAnywhere, Category = "Boid|Avoidance", meta = (ClampMin = "0", ClampMax = "1000"))
	int32 numSensors;
	//golden ratio constant used for spacing the packing points onto the sphere
	const float goldenRatio = (1.0f + FMath::Sqrt(5.0f)) / 2;
	//range of the avoidance collision sensors
	UPROPERTY(EditAnywhere, Category = "Boid|Avoidance", meta = (ClampMin = "0"))
	float sensorRadius;
	//avoidance sensor directions
	TArray<FVector> avoidanceSensors;
	//creates the directions that the avoidance sensors point
	void BuildAvoidanceSensors();

public:
	//getters + setters
	inline float GetSensorRadius() { return sensorRadius; }
	inline TArray<FVector> GetAvoidanceSensors() { return avoidanceSensors; }
};
