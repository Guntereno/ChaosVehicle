//#include "VehiclePlayerController.h"
//
//#include "EnhancedInputComponent.h"
//#include "EnhancedInputSubsystems.h"
//
//
//DEFINE_LOG_CATEGORY(LogVehiclePlayerController);
//
//void AVehiclePlayerController::BeginPlay()
//{
//	APlayerController::BeginPlay();
//
//	if (!IsValid(InputMappingContext))
//	{
//		UE_LOG(LogVehiclePlayerController, Error, TEXT("InputMappingContext property is invalid!"));
//		return;
//	}
//
//#if WITH_EDITOR
//	if (!Cast<UEnhancedPlayerInput>(PlayerInput))
//	{
//		UE_LOG(LogVehiclePlayerController, Error, TEXT("Player controller is not using the Enhanced Input system!"));
//		return;
//	}
//#endif	// WITH_EDITOR
//
//	UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
//	if (!IsValid(EnhancedInputLocalPlayerSubsystem))
//	{
//		UE_LOG(LogVehiclePlayerController, Error, TEXT("Failed to get UEnhancedInputLocalPlayerSubsystem."));
//		return;
//	}
//
//	EnhancedInputLocalPlayerSubsystem->AddMappingContext(InputMappingContext, 0);
//
//
//	FStringClassReference MyWidgetClassRef(TEXT(“/Game/NewWidgetBlueprint.NewWidgetBlueprint_C”));
//	if (UClass* MyWidgetClass = MyWidgetClassRef.TryLoadClass<UUserWidget>())
//	{
//		UUserWidget* MyWidget = CreateWidget<UUserWidget>(World / GameInstance / PlayerController, MyWidgetClass);
//	}
//}