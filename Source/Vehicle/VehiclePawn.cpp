#include "VehiclePawn.h"

#include "Camera/CameraComponent.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "Components/PrimitiveComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/NavMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


DEFINE_LOG_CATEGORY(LogVehiclePawn);

static const FName BrakeLightPowerName = FName(TEXT("BrakeLightPower"));


AVehiclePawn::AVehiclePawn()
{
	PrimaryActorTick.bCanEverTick = true;


	FrontSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmFront"));
	FrontSpringArm->SetRelativeLocationAndRotation(
		FVector(30.0f, 0.0f, 120.0f),
		FRotator(0.0f, 0.0f, 0.0f));
	FrontSpringArm->TargetArmLength = 0.0f;
	FrontSpringArm->bEnableCameraLag = false;
	FrontSpringArm->bDoCollisionTest = false;
	FrontSpringArm->bEnableCameraRotationLag = true;
	FrontSpringArm->CameraRotationLagSpeed = 15.0f;
	FrontSpringArm->SetupAttachment(RootComponent);

	FrontCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraFront"));
	FrontCamera->SetupAttachment(FrontSpringArm, USpringArmComponent::SocketName);
	FrontCamera->bAutoActivate = false;

	RearSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmRear"));
	RearSpringArm->SetupAttachment(RootComponent);
	RearSpringArm->SetRelativeLocationAndRotation(
		FVector(0.0f, 0.0f, 75.0f),
		FRotator(0.0f, 0.0f, 0.0f));
	RearSpringArm->TargetArmLength = 650.0f;
	RearSpringArm->SocketOffset = FVector(0.0f, 0.0f, 150.0f);
	RearSpringArm->bDoCollisionTest = false;
	RearSpringArm->bInheritPitch = false;
	RearSpringArm->bInheritRoll = false;
	RearSpringArm->bEnableCameraLag = true;
	RearSpringArm->CameraLagSpeed = 2.0f;
	RearSpringArm->CameraLagMaxDistance = 50.0f;
	RearSpringArm->SetupAttachment(RootComponent);

	RearCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraRear"));
	RearCamera->SetupAttachment(RearSpringArm, USpringArmComponent::SocketName);

	UChaosVehicleMovementComponent* ChaosVehicleMovementComponent = GetVehicleMovement();
	UChaosWheeledVehicleMovementComponent* ChaosWheeledVehicleMovementComponent = Cast<UChaosWheeledVehicleMovementComponent>(ChaosVehicleMovementComponent);
	if (IsValid(ChaosWheeledVehicleMovementComponent))
	{
		ChaosWheeledVehicleMovementComponent->bLegacyWheelFrictionPosition = true;
	}

	USkeletalMeshComponent* Primitive = GetMesh();
	Primitive->SetSimulatePhysics(true);
}

void AVehiclePawn::BeginPlay()
{
	Super::BeginPlay();

	CreateMaterialInstance();
}

// Called every frame
void AVehiclePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const UChaosVehicleMovementComponent* Movement = GetVehicleMovement();
	USkeletalMeshComponent* Primitive = GetMesh();
	if (IsValid(Movement) && IsValid(Primitive))
	{
		float AngularDamping = Movement->IsMovingOnGround() ? 0.0f : 1.0f;
		Primitive->SetAngularDamping(AngularDamping);
	}

	TickCamera(DeltaTime);
}

// Called to bind functionality to input
void AVehiclePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!IsValid(Input))
	{
		UE_LOG(LogVehiclePawn, Error, TEXT("PlayerInputComponent parameter is invalid!"));
		return;
	}

	if (IsValid(ThrottleAction))
	{
		Input->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &AVehiclePawn::ThrottleTriggered);
		Input->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &AVehiclePawn::ThrottleTriggered);
	}
	else
	{
		UE_LOG(LogVehiclePawn, Error, TEXT("ThrottleAction property is invalid in %s!"), *GetName());
	}

	if (IsValid(BrakeAction))
	{
		Input->BindAction(BrakeAction, ETriggerEvent::Started, this, &AVehiclePawn::BrakeStarted);
		Input->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &AVehiclePawn::UpdateBrake);
		Input->BindAction(BrakeAction, ETriggerEvent::Completed, this, &AVehiclePawn::BrakeCompleted);
	}
	else
	{
		UE_LOG(LogVehiclePawn, Error, TEXT("BrakeAction property is invalid in %s!"), *GetName());
	}

	if (IsValid(SteeringAction))
	{
		Input->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &AVehiclePawn::SteeringTriggered);
		Input->BindAction(SteeringAction, ETriggerEvent::Completed, this, &AVehiclePawn::SteeringTriggered);
	}
	else
	{
		UE_LOG(LogVehiclePawn, Error, TEXT("SteeringAction property is invalid in %s!"), *GetName());
	}

	if (IsValid(HandbrakeAction))
	{
		Input->BindAction(HandbrakeAction, ETriggerEvent::Triggered, this, &AVehiclePawn::HandbrakeTriggered);
		Input->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &AVehiclePawn::HandbrakeCompleted);
	}
	else
	{
		UE_LOG(LogVehiclePawn, Error, TEXT("BrakeAction property is invalid in %s!"), *GetName());
	}

	if (IsValid(LookAroundAction))
	{
		Input->BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &AVehiclePawn::LookAroundTriggered);
	}
	else
	{
		UE_LOG(LogVehiclePawn, Error, TEXT("BrakeAction property is invalid in %s!"), *GetName());
	}

	if (IsValid(ToggleCameraAction))
	{
		Input->BindAction(ToggleCameraAction, ETriggerEvent::Triggered, this, &AVehiclePawn::ToggleCameraTriggered);
	}
	else
	{
		UE_LOG(LogVehiclePawn, Error, TEXT("ToggleCameraAction property is invalid in %s!"), *GetName());
	}

	if (IsValid(ResetAction))
	{
		Input->BindAction(ResetAction, ETriggerEvent::Triggered, this, &AVehiclePawn::ResetTriggered);
	}
	else
	{
		UE_LOG(LogVehiclePawn, Error, TEXT("ResetAction property is invalid in %s!"), *GetName());
	}
}


void AVehiclePawn::CreateMaterialInstance()
{
	USkeletalMeshComponent* Primitive = GetMesh();
	if (!IsValid(Primitive))
	{
		UE_LOG(LogVehiclePawn, Error, TEXT("GetMesh returns invalid pointer!"));
		return;
	}

	constexpr int ChassisMaterialIndex = 0;
	ChassisMaterialInterface = Primitive->GetMaterial(ChassisMaterialIndex);
	if (!IsValid(ChassisMaterialInterface))
	{
		UE_LOG(LogVehiclePawn, Error, TEXT("Failed to get material at index %d!"), ChassisMaterialIndex);
		return;
	}

	ChassisDynamicMaterial = UMaterialInstanceDynamic::Create(ChassisMaterialInterface, this);
	if (!IsValid(ChassisDynamicMaterial))
	{
		UE_LOG(LogVehiclePawn, Error, TEXT("Failed to create dynamic material!"));
		return;
	}

	Primitive->SetMaterial(ChassisMaterialIndex, ChassisDynamicMaterial);
}


void AVehiclePawn::ThrottleTriggered(const FInputActionInstance& Instance)
{
	float FloatValue = Instance.GetValue().Get<float>();
	UChaosVehicleMovementComponent* Movement = GetVehicleMovement();
	if (IsValid(Movement))
	{
		Movement->SetThrottleInput(FloatValue);
	}
}

void AVehiclePawn::UpdateBrake(const FInputActionInstance& Instance)
{
	float FloatValue = Instance.GetValue().Get<float>();
	UpdateBrake(FloatValue);
}

void AVehiclePawn::UpdateBrake(const float BrakeValue)
{
	UChaosVehicleMovementComponent* Movement = GetVehicleMovement();
	if (IsValid(Movement))
	{
		Movement->SetBrakeInput(BrakeValue);
	}
}

void AVehiclePawn::BrakeStarted(const FInputActionInstance& Instance)
{
	SetBrakeLights(true);
}

void AVehiclePawn::BrakeCompleted(const FInputActionInstance& Instance)
{
	SetBrakeLights(false);
	UpdateBrake(0.0f);
}

void AVehiclePawn::SteeringTriggered(const FInputActionInstance& Instance)
{
	float FloatValue = Instance.GetValue().Get<float>();
	UChaosVehicleMovementComponent* Movement = GetVehicleMovement();
	if (IsValid(Movement))
	{
		Movement->SetSteeringInput(FloatValue);
	}
}

void AVehiclePawn::HandbrakeTriggered(const FInputActionInstance& Instance)
{
	bool bBoolValue = Instance.GetValue().Get<bool>();
	UpdateHandbrake(bBoolValue);

	SetBrakeLights(true);
}

void AVehiclePawn::HandbrakeCompleted(const FInputActionInstance& Instance)
{
	UpdateHandbrake(false);
	SetBrakeLights(false);
}


void AVehiclePawn::UpdateHandbrake(const bool bValue)
{
	UChaosVehicleMovementComponent* Movement = GetVehicleMovement();
	if (IsValid(Movement))
	{
		Movement->SetHandbrakeInput(bValue);
	}
}

void AVehiclePawn::LookAroundTriggered(const FInputActionInstance& Instance)
{
	float FloatValue = Instance.GetValue().Get<float>();

	USpringArmComponent* SpringArm = RearSpringArm.Get();
	if (!IsValid(SpringArm))
	{
		return;
	}

	const FRotator DeltaRotation = FRotator(0.0f, FloatValue, 0.0f);

	const FQuat CurRelRotQuat = SpringArm->GetRelativeRotationCache().RotatorToQuat(SpringArm->GetRelativeRotation());
	const FQuat NewRelRotQuat = CurRelRotQuat * DeltaRotation.Quaternion();

	SpringArm->SetRelativeRotation(NewRelRotQuat);
}

void AVehiclePawn::ToggleCameraTriggered(const FInputActionInstance& Instance)
{
	bool bRearCameraActive = RearCamera->IsActive();
	RearCamera->SetActive(!bRearCameraActive);
	FrontCamera->SetActive(bRearCameraActive);
}


void AVehiclePawn::ResetTriggered(const FInputActionInstance& Instance)
{
	ResetOrientation();
}

void AVehiclePawn::SetBrakeLights(const bool Enabled)
{
	ChassisDynamicMaterial->SetScalarParameterValue(BrakeLightPowerName, Enabled ? LightsOnIntensity : LightsOffIntensity);
}

void AVehiclePawn::TickCamera(const float DeltaTime)
{
	USpringArmComponent* SpringArm = RearSpringArm.Get();
	if (!IsValid(SpringArm))
	{
		return;
	}

	FRotator RelativeRotation = SpringArm->GetRelativeRotation();
	RelativeRotation.Yaw = FMath::FInterpTo(RelativeRotation.Yaw, 0.0f, DeltaTime, CamearaLerpToOriginSpeed);
	SpringArm->SetRelativeRotation(RelativeRotation);
}

void AVehiclePawn::ResetOrientation()
{
	FVector Location = GetActorLocation();
	Location.Z += ResetHeight;

	FRotator Rotation = GetActorRotation();
	Rotation.Pitch = Rotation.Roll = 0.0f;

	FTransform NewTransform = FTransform(Rotation, Location);
	SetActorTransform(NewTransform, false, nullptr, ETeleportType::TeleportPhysics);

	USkeletalMeshComponent* Primitive = GetMesh();
	Primitive->SetAllPhysicsAngularVelocityInRadians(FVector::Zero());
	Primitive->SetAllPhysicsLinearVelocity(FVector::Zero());
}