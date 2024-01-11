#pragma once

#include "CoreMinimal.h"
#include "VehicleCameraRig.generated.h"

class UCameraComponent;
class USpringArmComponent;

USTRUCT()
struct FVehicleCameraRig
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArm = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> Camera = nullptr;
};
