// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NNPPlayerController.generated.h"

@class GCController;
@class CHHapticEngine;


typedef enum NNP_BUTTONS
{
	AButton = 0,
	BButton,
	XButton,
	YButton,
	LShoulder,
	RShoulder,
	LTrigger,
	RTrigger,
	
	MAX_CONTROLLER_BUTTONS
} NNPButtons;

typedef enum NNP_HAPTIC_EVENTS
{
	Weak_Haptics = 0,
	Medium_Haptics,
	Heavy_Haptics,
	Full_Haptics,
	
	MAX_HAPTIC_EVENTS
}NNPHapticEvents;

typedef enum NNP_TOUCHSTICKS
{
	LTouchstick = 0,
	RTouchstick,
	
	MAX_TOUCH_STICKS
} NNPTouchsticks;

struct Touchstick
{
	bool FirstTouch;
	float Radius;
	FVector2D Center;
	FVector2D StickPos;
};

typedef void (*ButtonAction)(NNPButtons button, void *object, bool pressed);


/**
 * 
 */
UCLASS()
class NNP_BITFRYTESTDEMO_API ANNPPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ANNPPlayerController();
	
	// Initialize a hardware controller, if possible.  Returns true if
	// a hardware controller is initialized and false otherwise.
	bool InitializeHardwareController(FRotator orientation);
	
	bool IsInitialized();
	
	// Perform the actual gamepad setup.
	void ToggleHardwareController(bool useHardware);
	
	// Get the current orientation of the controller.
	FRotator GetOrientation();
	
	// Update the controller's orientation by the given amount of yaw, pitch, or roll.
	void AddYawInput(float value);
	void AddPitchInput(float value);
	void AddRollInput(float value);
	
	
	// Get the (x, y) values of the given Thumbstick.
	FVector2D GetThumbstick(bool leftStick = true);
	
	void SetTouchstick(APlayerController *contoller);
	FVector2D GetTouchstick(bool leftStick = true);
	
	// Set a callback for a button.
	void SetButtonAction(NNPButtons button, void* object, ButtonAction callback);
	// Get the current value of the requested button.
	float GetButton(NNPButtons button);
	
	// Update Haptics
	void UpdateHaptics(float intensity, float sharpness);
	
protected:
	GCController *Controller;
	CHHapticEngine *Haptics;
	NSTimer *HapticsTimer;
	id HapticsPlayer;
	
	bool InitComplete;
	
	float Buttons[MAX_CONTROLLER_BUTTONS];
	ButtonAction ButtonActions[MAX_CONTROLLER_BUTTONS];
	void* CallingObject[MAX_CONTROLLER_BUTTONS];
	
	Touchstick Touchsticks[MAX_TOUCH_STICKS];
	FVector2D RThumbstick;
	FVector2D LThumbstick;
	FRotator ControllerOrientation;
	
	void InitializeHaptics() API_AVAILABLE(ios(14));
	
};
