

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "WheeledVehiclePawn.h"
#include "VehiclePawn.generated.h"

class UCameraComponent;
class UChaosWheeledVehicleMovementComponent;
class UInputAction;
class UNavMovementComponent;
class UPrimitiveComponent;
class USpringArmComponent;

struct FInputActionInstance;


DECLARE_LOG_CATEGORY_EXTERN(LogVehiclePawn, Log, All);


UCLASS()
class VEHICLE_API AVehiclePawn : public AWheeledVehiclePawn
{
	GENERATED_BODY()

public:
	AVehiclePawn();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

private:
	// Height at which to spawn the vehicle when resetting 
	UPROPERTY(EditAnywhere, Category = "Reset", meta = (AllowPrivateAccess = "true"), meta=(units=m))
	float ResetHeight = 50.0f;

	// Speed at which the rear camera arm lerps towards the origin
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float CamearaLerpToOriginSpeed = 1.0f;

	// Intensity of the vehicle lamps when off
	UPROPERTY(EditAnywhere, Category = "Lights", meta = (AllowPrivateAccess = "true"))
	float LightsOffIntensity = 4.0f;

	// Intensity of the vehicle lamps when on
	UPROPERTY(EditAnywhere, Category = "Lights", meta = (AllowPrivateAccess = "true"))
	float LightsOnIntensity = 200.0f;


	UPROPERTY(EditAnywhere, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<const UInputAction> ThrottleAction = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<const UInputAction> BrakeAction = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<const UInputAction> SteeringAction = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<const UInputAction> HandbrakeAction = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<const UInputAction> LookAroundAction = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<const UInputAction> ToggleCameraAction = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<const UInputAction> ResetAction = nullptr;


	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> FrontSpringArm = nullptr;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FrontCamera = nullptr;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> RearSpringArm = nullptr;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> RearCamera = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInterface> ChassisMaterialInterface = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> ChassisDynamicMaterial = nullptr;


	void CreateMaterialInstance();


	void ThrottleTriggered(const FInputActionInstance& Instance);

	void BrakeStarted(const FInputActionInstance& Instance);
	void BrakeCompleted(const FInputActionInstance& Instance);
	void UpdateBrake(const FInputActionInstance& Instance);
	void UpdateBrake(const float BrakeValue);

	void HandbrakeTriggered(const FInputActionInstance& Instance);
	void HandbrakeCompleted(const FInputActionInstance& Instance);
	void UpdateHandbrake(const bool bValue);

	void LookAroundTriggered(const FInputActionInstance& Instance);

	void SteeringTriggered(const FInputActionInstance& Instance);

	void ToggleCameraTriggered(const FInputActionInstance& Instance);

	void ResetTriggered(const FInputActionInstance& Instance);

	
	void SetBrakeLights(const bool Enabled);

	void TickCamera(const float DeltaTime);

	void ResetOrientation();
};
