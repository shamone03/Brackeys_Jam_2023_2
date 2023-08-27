#include "FlockManager.h"
#include "Components/BillboardComponent.h"
#include "Boid.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AFlockManager::AFlockManager()
{
	//disable ticking
	PrimaryActorTick.bCanEverTick = false;

	//setup billboard visual component
	flockManagerBillboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("FlockManager Billboard Component"));
	RootComponent = flockManagerBillboard;

	//TODO: set to "general" preset settings that are a good starting default
	//set speed settings
	maxSpeed = 700.0f;
	minSpeed = 300.0f;

	//default behavioral steering strengths
	alignmentStrength = 200.0f;
	separationStrength = 30.0f;
	cohesionStrength = 5.0f;
	avoidanceStrength = 10000.0f;

	//default perception angles
	separationFOV = -1.0f;
	alignmentFOV = 0.5f;
	cohesionFOV = -0.5f;

	//default avoidance properties
	numSensors = 100;
	sensorRadius = 300.0f;
	BuildAvoidanceSensors();
}

// Called when the game starts or when spawned
void AFlockManager::BeginPlay()
{
	Super::BeginPlay();
}

void AFlockManager::AddBoidToFlock(ABoid* boid)
{
	if (boid) {
		boidsInFlock.AddUnique(boid);
	}
}

void AFlockManager::RemoveBoidFromFlock(ABoid* boid)
{
	if (boid) {
		boidsInFlock.Remove(boid);
	}
}

void AFlockManager::SetMaxSpeed(float newMaxSpeed)
{
	if (newMaxSpeed < 0)
	{
		//log warning to console
		UE_LOG(LogTemp, Warning, TEXT("Request to change Boid Max Speed to negative value ignored in FlockManager: %s."), *GetName());
		return;
	}

	//update max speed to new setting
	maxSpeed = newMaxSpeed;

	//check if new Max is lower than Min, lower Min to new speed to avoid Min > Max error behavior
	if (maxSpeed < minSpeed)
	{
		minSpeed = maxSpeed;
	}
}

void AFlockManager::SetMinSpeed(float newMinSpeed)
{
	if (newMinSpeed < 0)
	{
		//log warning to console
		UE_LOG(LogTemp, Warning, TEXT("Request to change Boid Min Speed to negative value ignored in FlockManager: %s."), *GetName());
		return;
	}

	//update max speed to new setting
	minSpeed = newMinSpeed;

	//check if new Max is lower than Min, lower Min to new speed to avoid Min > Max error behavior
	if (maxSpeed < minSpeed)
	{
		maxSpeed = minSpeed;
	}
}

void AFlockManager::SetAlignmentStrength(float newAlignmentStrength)
{
	alignmentStrength = newAlignmentStrength;
}

void AFlockManager::SetSeparationStrength(float newSeparationStrength)
{
	separationStrength = newSeparationStrength;
}

void AFlockManager::SetCohesionStrength(float newCohesionStrength)
{
	cohesionStrength = newCohesionStrength;
}

void AFlockManager::SetAvoidanceStrength(float newAvoidanceStrength)
{
	avoidanceStrength = newAvoidanceStrength;
}

void AFlockManager::BuildAvoidanceSensors()
{
	avoidanceSensors.Empty();

	//theta angle of rotation on xy plane around z axis (yaw) around sphere
	float theta;
	//phi angle of rotation (~pitch) around sphere
	float phi;
	//assuming unit sphere (radius = 1) to simplify calculations
	//direction vector pointing from the center to point on sphere surface
	FVector SensorDirection;

	for (int32 i = 0; i < numSensors; ++i)
	{
		//calculate the spherical coordinates of the direction vectors endpoint
		theta = 2 * UKismetMathLibrary::GetPI() * goldenRatio * i;
		phi = FMath::Acos(1 - (2 * float(i) / numSensors));

		//convert point on sphere to cartesian coordinates xyz
		SensorDirection.X = FMath::Cos(theta)*FMath::Sin(phi);
		SensorDirection.Y = FMath::Sin(theta)*FMath::Sin(phi);
		SensorDirection.Z = FMath::Cos(phi);
		//add direction to list of sensors for avoidance
		avoidanceSensors.Emplace(SensorDirection);
	}
}
