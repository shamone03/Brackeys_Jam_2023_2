// Fill out your copyright notice in the Description page of Project Settings.

#include "BoidCageSpawner.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Boid.h"
#include "FlockManager.h"

// Sets default values
ABoidCageSpawner::ABoidCageSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	_cageCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Cage Collision Component"));
	RootComponent = _cageCollision;
	_cageCollision->SetBoxExtent(FVector(2500));
	_cageCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	_cageCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	_cageCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	_cageMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cage Mesh Component"));
	_cageMesh->SetupAttachment(RootComponent);
	_cageMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	_cageMesh->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));

	_numBoidsToSpawn = 0;
}

// Called when the game starts or when spawned
void ABoidCageSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (_cageCollision) {
		_cageCollision->OnComponentEndOverlap.AddDynamic(this, &ABoidCageSpawner::OnCageOverlapEnd);
	}
	
	SpawnBoids(_numBoidsToSpawn);
}

void ABoidCageSpawner::OnCageOverlapEnd(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex)
{
	ABoid* escapingBoid = Cast<ABoid>(otherActor);
	if (escapingBoid) {
		if (escapingBoid->IsActorBeingDestroyed()) {
			return;
		}

		FVector newBoidLocation = escapingBoid->GetActorLocation();

		if (escapingBoid->GetActorLocation().X > this->GetActorLocation().X + _cageCollision->GetScaledBoxExtent().X)
		{
			//move boid to back side
			newBoidLocation.X = this->GetActorLocation().X - _cageCollision->GetScaledBoxExtent().X;
			newBoidLocation.Y = FMath::Clamp(newBoidLocation.Y, this->GetActorLocation().Y - _cageCollision->GetScaledBoxExtent().Y, this->GetActorLocation().Y + _cageCollision->GetScaledBoxExtent().Y);
			newBoidLocation.Z = FMath::Clamp(newBoidLocation.Z, this->GetActorLocation().Z - _cageCollision->GetScaledBoxExtent().Z, this->GetActorLocation().Z + _cageCollision->GetScaledBoxExtent().Z);
			escapingBoid->SetActorLocation(newBoidLocation);
		}
		//exited back side
		else if (escapingBoid->GetActorLocation().X < this->GetActorLocation().X - _cageCollision->GetScaledBoxExtent().X)
		{
			//move boid to front side
			newBoidLocation.X = this->GetActorLocation().X + _cageCollision->GetScaledBoxExtent().X;
			newBoidLocation.Y = FMath::Clamp(newBoidLocation.Y, this->GetActorLocation().Y - _cageCollision->GetScaledBoxExtent().Y, this->GetActorLocation().Y + _cageCollision->GetScaledBoxExtent().Y);
			newBoidLocation.Z = FMath::Clamp(newBoidLocation.Z, this->GetActorLocation().Z - _cageCollision->GetScaledBoxExtent().Z, this->GetActorLocation().Z + _cageCollision->GetScaledBoxExtent().Z);
			escapingBoid->SetActorLocation(newBoidLocation);
		}
		//exited right side
		else if (escapingBoid->GetActorLocation().Y > this->GetActorLocation().Y + _cageCollision->GetScaledBoxExtent().Y)
		{
			//move boid to left side
			newBoidLocation.X = FMath::Clamp(newBoidLocation.X, this->GetActorLocation().X - _cageCollision->GetScaledBoxExtent().X, this->GetActorLocation().X + _cageCollision->GetScaledBoxExtent().X);
			newBoidLocation.Y = this->GetActorLocation().Y - _cageCollision->GetScaledBoxExtent().Y;
			newBoidLocation.Z = FMath::Clamp(newBoidLocation.Z, this->GetActorLocation().Z - _cageCollision->GetScaledBoxExtent().Z, this->GetActorLocation().Z + _cageCollision->GetScaledBoxExtent().Z);
			escapingBoid->SetActorLocation(newBoidLocation);
		}
		//exited left side
		else if (escapingBoid->GetActorLocation().Y < this->GetActorLocation().Y - _cageCollision->GetScaledBoxExtent().Y)
		{
			//move boid to right side
			newBoidLocation.X = FMath::Clamp(newBoidLocation.X, this->GetActorLocation().X - _cageCollision->GetScaledBoxExtent().X, this->GetActorLocation().X + _cageCollision->GetScaledBoxExtent().X);
			newBoidLocation.Y = this->GetActorLocation().Y + _cageCollision->GetScaledBoxExtent().Y;
			newBoidLocation.Z = FMath::Clamp(newBoidLocation.Z, this->GetActorLocation().Z - _cageCollision->GetScaledBoxExtent().Z, this->GetActorLocation().Z + _cageCollision->GetScaledBoxExtent().Z);
			escapingBoid->SetActorLocation(newBoidLocation);
		}
		//exited top side
		else if (escapingBoid->GetActorLocation().Z > this->GetActorLocation().Z + _cageCollision->GetScaledBoxExtent().Z)
		{
			//move boid to bottom side
			newBoidLocation.X = FMath::Clamp(newBoidLocation.X, this->GetActorLocation().X - _cageCollision->GetScaledBoxExtent().X, this->GetActorLocation().X + _cageCollision->GetScaledBoxExtent().X);
			newBoidLocation.Y = FMath::Clamp(newBoidLocation.Y, this->GetActorLocation().Y - _cageCollision->GetScaledBoxExtent().Y, this->GetActorLocation().Y + _cageCollision->GetScaledBoxExtent().Y);
			newBoidLocation.Z = this->GetActorLocation().Z - _cageCollision->GetScaledBoxExtent().Z;
			escapingBoid->SetActorLocation(newBoidLocation);
		}
		//exited bottom side
		else if (escapingBoid->GetActorLocation().Z < this->GetActorLocation().Z - _cageCollision->GetScaledBoxExtent().Z)
		{
			//move boid to top side
			newBoidLocation.X = FMath::Clamp(newBoidLocation.X, this->GetActorLocation().X - _cageCollision->GetScaledBoxExtent().X, this->GetActorLocation().X + _cageCollision->GetScaledBoxExtent().X);
			newBoidLocation.Y = FMath::Clamp(newBoidLocation.Y, this->GetActorLocation().Y - _cageCollision->GetScaledBoxExtent().Y, this->GetActorLocation().Y + _cageCollision->GetScaledBoxExtent().Y);
			newBoidLocation.Z = this->GetActorLocation().Z + _cageCollision->GetScaledBoxExtent().Z;
			escapingBoid->SetActorLocation(newBoidLocation);
		}
		//unexpected exit occurred, remove boid from flock, destroy it, and spawn new one
		else
		{
			escapingBoid->GetFlockManager()->RemoveBoidFromFlock(escapingBoid);
			GetWorld()->DestroyActor(escapingBoid);
			SpawnBoids(1);
		}
	}
}

void ABoidCageSpawner::SpawnBoids(int numBoids)
{
	if (_assignedFlockManager == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("No FlockManager found for Boid spawner: %s."), *GetName());
		return;
	}

	if (_boidType == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("BoidType not set for Spawner: %s"), *GetName());
		return;
	}

	FVector spawnLocation = FVector::ZeroVector;

	FVector rotationDirection = FVector::ZeroVector;
	FRotator spawnRotation = FRotator::ZeroRotator;

	FActorSpawnParameters boidSpawnParams;
	boidSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	boidSpawnParams.Owner = _assignedFlockManager;

	for (int i = 0; i < numBoids; i++) {
		spawnLocation.X = 0.0f;
		spawnLocation.Y = FMath::FRandRange(-_cageCollision->GetScaledBoxExtent().X / 2, _cageCollision->GetScaledBoxExtent().X / 2);
		spawnLocation.Z = FMath::FRandRange(-_cageCollision->GetScaledBoxExtent().Z / 2, _cageCollision->GetScaledBoxExtent().Z / 2);
		spawnLocation += this->GetActorLocation();

		rotationDirection = FMath::VRand();
		rotationDirection.X = 0.0;

		spawnRotation = rotationDirection.ToOrientationRotator();

		_assignedFlockManager->AddBoidToFlock(GetWorld()->SpawnActor<ABoid>(_boidType, spawnLocation, spawnRotation, boidSpawnParams));
	}
}