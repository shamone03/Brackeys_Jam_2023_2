#include "Boid.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "FlockManager.h"

// Sets default values
ABoid::ABoid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_boidCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Boid Collision Component"));
	RootComponent = _boidCollider;
	_boidCollider->SetCollisionObjectType(ECC_Pawn);
	_boidCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	_boidCollider->SetCollisionResponseToAllChannels(ECR_Overlap);

	_boidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Boid Mesh Component"));
	_boidMesh->SetupAttachment(RootComponent);
	_boidMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_boidMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	_perceptualField = CreateDefaultSubobject<USphereComponent>(TEXT("Perception Sensor Component"));
	_perceptualField->SetupAttachment(RootComponent);
	_perceptualField->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	_perceptualField->SetCollisionResponseToAllChannels(ECR_Ignore);
	_perceptualField->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	_perceptualField->SetSphereRadius(300.0f);

	_boidVelocity = FVector::ZeroVector;
}

// Called when the game starts or when spawned
void ABoid::BeginPlay()
{
	Super::BeginPlay();

	AFlockManager* boidOwner = Cast<AFlockManager>(this->GetOwner());
	if (boidOwner) {
		_flockManager = boidOwner;

		_boidVelocity = this->GetActorForwardVector();
		_boidVelocity.Normalize();
		_boidVelocity *= FMath::FRandRange(_flockManager->GetMinSpeed(), _flockManager->GetMaxSpeed());

		_currentRotation = this->GetActorRotation();
	}
	else {
		// Oh no ahhhhhhhhhhhhhhhhhhhhhhh
		UE_LOG(LogTemp, Warning, TEXT("No FlockManager found for Boid: %s."), *GetName());
	}
}

// Called every frame
void ABoid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// determine boid movement direction and velocity
	Steer(DeltaTime);

	// update the rotation of the mesh
	UpdateMeshRotation();
}

void ABoid::UpdateMeshRotation()
{
	_currentRotation = FMath::RInterpTo(_currentRotation, this->GetActorRotation(), GetWorld()->DeltaTimeSeconds, 7.0f);
	this->_boidMesh->SetWorldRotation(_currentRotation);
}

FVector ABoid::Separate(TArray<ABoid*> flock)
{
	if (_flockManager == nullptr || flock.Num() == 0) {
		return FVector::ZeroVector;
	}

	FVector steering = FVector::ZeroVector;
	FVector separationDirection = FVector::ZeroVector;
	float proximityFactor = 0.0f;

	for (ABoid* flockmate : flock) {
		if (flockmate != nullptr && flockmate != this) {
			if (FVector::DotProduct(this->GetActorForwardVector(), (flockmate->GetActorLocation() - this->GetActorLocation()).GetSafeNormal()) <= _flockManager->GetSeparationFOV()) {
				continue;
			}

			separationDirection = (this->GetActorLocation() - flockmate->GetActorLocation());

			proximityFactor = 1.0f - (separationDirection.Size() / this->_perceptualField->GetScaledSphereRadius());

			//UE_LOG(LogTemp, Warning, TEXT("sphere Radius %f."), this->_perceptualField->GetScaledSphereRadius());
			//UE_LOG(LogTemp, Warning, TEXT("separationDirection Size %f."), separationDirection.Size());
			//UE_LOG(LogTemp, Warning, TEXT("seperation direction %s."), *separationDirection.ToString());

			if (proximityFactor < 0.0f) {
				continue;
			}

			steering += (proximityFactor * separationDirection);
		}
	}

	steering /= flock.Num();
	steering.GetSafeNormal() -= this->_boidVelocity.GetSafeNormal();
	steering *= _flockManager->GetSeparationStrength();
	return steering;
}

FVector ABoid::Align(TArray<ABoid*> flock)
{
	if (_flockManager == nullptr || flock.Num() == 0) {
		return FVector::ZeroVector;
	}

	FVector steering = FVector::ZeroVector;

	for (ABoid* flockmate : flock) {
		if (flockmate != nullptr && flockmate != this) {
			if (FVector::DotProduct(this->GetActorForwardVector(), (flockmate->GetActorLocation() - this->GetActorLocation()).GetSafeNormal()) <= _flockManager->GetAlignmentFOV()) {
				continue;
			}

			steering += flockmate->_boidVelocity.GetSafeNormal();
		}
	}

	steering /= flock.Num();
	steering.GetSafeNormal() -= this->_boidVelocity.GetSafeNormal();
	steering *= _flockManager->GetAlignmentStrength();
	return steering;
}

FVector ABoid::GroupUp(TArray<ABoid*> flock)
{
	if (_flockManager == nullptr || flock.Num() == 0) {
		return FVector::ZeroVector;
	}

	FVector steering = FVector::ZeroVector;
	FVector averagePosition = FVector::ZeroVector;
	int32 count = 0;

	for (ABoid* flockmate : flock) {
		if (flockmate != nullptr && flockmate != this) {
			if (FVector::DotProduct(this->GetActorForwardVector(), (flockmate->GetActorLocation() - this->GetActorLocation()).GetSafeNormal()) <= _flockManager->GetCohesionFOV())
			{
				continue;
			}

			averagePosition += flockmate->GetActorLocation();
			count++;
		}
	}

	averagePosition /= count;
	steering = averagePosition - this->GetActorLocation();
	//steering.GetSafeNormal() -= this->_boidVelocity.GetSafeNormal();
	//steering *= _flockManager->GetCohesionStrength();

	UE_LOG(LogTemp, Warning, TEXT("Average Position: %s. \n Current Actor Position: %s \n Force Direction %s"), *averagePosition.ToString(), *this->GetActorLocation().ToString(), *steering.ToString());

	return steering;
}

void ABoid::Steer(float DeltaTime)
{
	if (_flockManager == nullptr) {
		return;
	}

	FVector acceleration = FVector::ZeroVector;

	TArray<AActor*> actors;
	_perceptualField->GetOverlappingActors(actors, TSubclassOf<ABoid>());

	TArray<ABoid*> flockmates;
	for (AActor* actor : actors) {
		ABoid* boid = Cast<ABoid>(actor);
		if (boid) {
			flockmates.Add(boid);
		}
	}

	acceleration += Separate(flockmates);
	acceleration += Align(flockmates);
	acceleration += GroupUp(flockmates);

	//if (IsObstacleAhead()) {
	//	acceleration = AvoidObstacle();
	//}

	for (FVector TargetForce : _targetForces)
	{
		acceleration += TargetForce;
		_targetForces.Remove(TargetForce);
	}

	acceleration.X = 0;

	_boidVelocity += (acceleration * DeltaTime);
	_boidVelocity = _boidVelocity.GetClampedToSize(_flockManager->GetMinSpeed(), _flockManager->GetMaxSpeed());

	this->SetActorLocation(this->GetActorLocation() + (_boidVelocity * DeltaTime));
	this->SetActorRotation(_boidVelocity.ToOrientationQuat());
}

bool ABoid::IsObstacleAhead()
{
	if (_flockManager == nullptr) {
		return false;
	}

	if (_flockManager->GetAvoidanceSensors().Num() > 0) {
		FQuat SensorRotation = FQuat::FindBetweenVectors(_flockManager->GetAvoidanceSensors()[0], this->GetActorForwardVector());
		FVector newSensorDirection = SensorRotation.RotateVector(_flockManager->GetAvoidanceSensors()[0]);
		FVector traceEndPoint = this->GetActorLocation() + newSensorDirection * _flockManager->GetSensorRadius();
		FCollisionQueryParams traceParameters;
		FHitResult hit;

		GetWorld()->LineTraceSingleByChannel(hit, this->GetActorLocation(), traceEndPoint, COLLISION_AVOIDANCE, traceParameters);

		if (hit.bBlockingHit)
		{
			TArray<AActor*> OverlapActors;
			_boidCollider->GetOverlappingActors(OverlapActors);
			for (AActor* OverlapActor : OverlapActors)
			{
				if (hit.GetActor() == OverlapActor)
				{
					return false;
				}
			}
		}

		return hit.bBlockingHit;
	}

	return false;
}

FVector ABoid::AvoidObstacle()
{
	if (_flockManager == nullptr) {
		return FVector::ZeroVector;
	}

	FVector steering = FVector::ZeroVector;
	FQuat sensorRotation = FQuat::FindBetweenVectors(_flockManager->GetAvoidanceSensors()[0], this->GetActorForwardVector());
	FVector newSensorDirection = FVector::ZeroVector;
	FCollisionQueryParams traceParameters;
	FHitResult hit;

	for (FVector avoidanceSensor : _flockManager->GetAvoidanceSensors()) {
		newSensorDirection = sensorRotation.RotateVector(avoidanceSensor);
		GetWorld()->LineTraceSingleByChannel(hit, this->GetActorLocation(), this->GetActorLocation() + newSensorDirection * _flockManager->GetSensorRadius(), COLLISION_AVOIDANCE, traceParameters);
		if (!hit.bBlockingHit) {
			steering = newSensorDirection.GetSafeNormal() - this->_boidVelocity.GetSafeNormal();
			steering *= _flockManager->GetAvoidanceStrength();

		}
	}

	return FVector::ZeroVector;
}

void ABoid::AddTargetForce(FVector targetForce)
{
	_targetForces.Add(targetForce);
}