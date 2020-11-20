// Fill out your copyright notice in the Description page of Project Settings.

#include "NNPPlayerController.h"
#import <GameController/GameController.h>
#import <CoreHaptics/CoreHaptics.h>
#include "EngineGlobals.h"
#include "Engine/Engine.h"

#define TOUCH_STICK_RADIUS 100.0f

ANNPPlayerController::ANNPPlayerController() : Controller(nullptr), Haptics(nullptr), InitComplete(false)
{
	
}

// Initialize a hardware controller, if possible.  Returns true if
// a hardware controller is initialized and false otherwise.
bool ANNPPlayerController::InitializeHardwareController(FRotator orientation)
{
	int i;
	
	for(i = 0; i < MAX_CONTROLLER_BUTTONS; i++)
	{
		Buttons[i] = 0.0f;
		ButtonActions[i] = nullptr;
		CallingObject[i] = nullptr;
	}
	
	for(i = 0; i < MAX_TOUCH_STICKS; i++)
	{
		Touchsticks[i].FirstTouch = true;
		Touchsticks[i].Radius = TOUCH_STICK_RADIUS;
		Touchsticks[i].Center = {0.0f, 0.0f};
		Touchsticks[i].StickPos = {0.0f, 0.0f};
	}
	
	LThumbstick = {0.0f, 0.0f};
	RThumbstick = {0.0f, 0.0f};
	ControllerOrientation = orientation;
	
	if([[GCController controllers] count])
	{
		ToggleHardwareController(true);
		InitComplete = true;
		return true;
	}
	
	return false;
}

bool ANNPPlayerController::IsInitialized()
{
	return InitComplete;
}

// Perform the actual gamepad setup.
void ANNPPlayerController::ToggleHardwareController(bool useHardware)
{
	//if(GEngine)
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("ToggleHardwareController::(%i)"), useHardware));
	
	if(useHardware)
	{
		Controller = [GCController controllers][0];
		
		if(Controller.extendedGamepad)
		{
			// NNP: Set button press callbacks here.
			[Controller.extendedGamepad.buttonA setValueChangedHandler:^(GCControllerButtonInput *button, float value, BOOL pressed) {
				// Update button pressed information...
				if(pressed)
					Buttons[AButton] = value;
				else
					Buttons[AButton] = 0.0f;
				
				ButtonActions[AButton](AButton, CallingObject[AButton], pressed);
				
				//if(GEngine)
				//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("A Button: %f, pressed: %i"), value, pressed));
			}];
			[Controller.extendedGamepad.buttonB setValueChangedHandler:^(GCControllerButtonInput *button, float value, BOOL pressed) {
				// Update button pressed information...
				if(pressed)
					Buttons[BButton] = value;
				else
					Buttons[BButton] = 0.0f;
				
				ButtonActions[BButton](BButton, CallingObject[BButton], pressed);
			}];
			[Controller.extendedGamepad.buttonX setValueChangedHandler:^(GCControllerButtonInput *button, float value, BOOL pressed) {
				// Update button pressed information...
				if(pressed)
					Buttons[XButton] = value;
				else
					Buttons[XButton] = 0.0f;
				
				ButtonActions[XButton](XButton, CallingObject[XButton], pressed);
				
			}];
			[Controller.extendedGamepad.buttonY setValueChangedHandler:^(GCControllerButtonInput *button, float value, BOOL pressed) {
				// Update button pressed information...
				if(pressed)
					Buttons[YButton] = value;
				else
					Buttons[YButton] = 0.0f;
				
				ButtonActions[YButton](YButton, CallingObject[YButton], pressed);
			}];
			
			// NNP: setup shoulder and trigger button press callbacks here.
			[Controller.extendedGamepad.leftShoulder setValueChangedHandler:^(GCControllerButtonInput *button, float value, BOOL pressed) {
				// Update button pressed information...
				if(pressed)
					Buttons[LShoulder] = value;
				else
					Buttons[LShoulder] = 0.0f;
				
				ButtonActions[LShoulder](LShoulder, CallingObject[LShoulder], pressed);
			}];
			[Controller.extendedGamepad.rightShoulder setValueChangedHandler:^(GCControllerButtonInput *button, float value, BOOL pressed) {
				// Update button pressed information...
				if(pressed)
					Buttons[RShoulder] = value;
				else
					Buttons[RShoulder] = 0.0f;
				
				ButtonActions[RShoulder](RShoulder, CallingObject[RShoulder], pressed);
			}];
			[Controller.extendedGamepad.leftTrigger setValueChangedHandler:^(GCControllerButtonInput *button, float value, BOOL pressed) {
				// Update button pressed information...
				if(pressed)
					Buttons[LTrigger] = value;
				else
					Buttons[LTrigger] = 0.0f;
				
				ButtonActions[LTrigger](LTrigger, CallingObject[LTrigger], pressed);
			}];
			[Controller.extendedGamepad.rightTrigger setValueChangedHandler:^(GCControllerButtonInput *button, float value, BOOL pressed) {
				// Update button pressed information...
				if(pressed)
					Buttons[RTrigger] = value;
				else
					Buttons[RTrigger] = 0.0f;
				
				ButtonActions[RTrigger](RTrigger, CallingObject[RTrigger], pressed);
			}];
			
			// NNP: Set the thumbstick callbacks here.
			[Controller.extendedGamepad.leftThumbstick setValueChangedHandler:^(GCControllerDirectionPad *dPad, float xValue, float yValue){
				// Update our internal value for the left thumbstick.
				LThumbstick.X = xValue;
				LThumbstick.Y = yValue;
			}];
			
			[Controller.extendedGamepad.rightThumbstick setValueChangedHandler:^(GCControllerDirectionPad *dPad, float xValue, float yValue){
				// Update our internal value for the left thumbstick.
				RThumbstick.X = xValue;
				RThumbstick.Y = yValue;
			}];
		}
	}
	else
	{
		// NNP TODO: Do I need to do anything here?
	}
	
	// We have a controller so setup the haptics.
	//InitializeHaptics();
}

// Get the current orientation of the controller.
FRotator ANNPPlayerController::GetOrientation()
{
	return ControllerOrientation;
}

// Update the controller's orientation by the given amount of yaw, pitch, or roll.
void ANNPPlayerController::AddYawInput(float value)
{
	ControllerOrientation.Yaw += value;
}

void ANNPPlayerController::AddPitchInput(float value)
{
	ControllerOrientation.Pitch += value * -1.0f;
}

void ANNPPlayerController::AddRollInput(float value)
{
	//ControllerOrientation.Add(0.0f, 0.0f, value);
	ControllerOrientation.Roll += value;
}

// Get the (x, y) values of the given Thumbstick.
FVector2D ANNPPlayerController::GetThumbstick(bool leftStick)
{
	if(leftStick)
		return LThumbstick;
	
	return RThumbstick;
}

void ANNPPlayerController::SetTouchstick(APlayerController *controller)
{
	bool isPressed;
	bool leftStick;
	int index;
	float x;
	float y;
	FVector2D viewportSize;
	
	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("SetTouchsitck()"));
	
	if(!controller || !GEngine || !GEngine->GameViewport)
		return;
		
	GEngine->GameViewport->GetViewportSize(viewportSize);
	controller->GetInputTouchState(ETouchIndex::Touch1, x, y, isPressed);

	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("x: %f, y: %f, viewportSize: (%f, %f), isPressed: %i"), x, y, viewportSize.X, viewportSize.Y, isPressed));
	
	if(y > viewportSize.Y * 0.5f)
		return;
	
	if(x < viewportSize.X * 0.5f)
		leftStick = true;
	else
		leftStick = false;
	
	if(leftStick)
		index = LTouchstick;
	else
		index = RTouchstick;
	
	if(isPressed)
	{
		if(Touchsticks[index].FirstTouch)
		{
			Touchsticks[index].FirstTouch = false;
			Touchsticks[index].Center = {x, y};
		}
		
		Touchsticks[index].StickPos = {x, y};
	}
	else
	{
		Touchsticks[index].FirstTouch = true;
		Touchsticks[index].Center = {0.0f, 0.0f};
		Touchsticks[index].StickPos = {0.0f, 0.0f};
	}
}

FVector2D ANNPPlayerController::GetTouchstick(bool leftStick)
{
	int index;
	float distance;
	FVector2D direction;
	
	if(leftStick)
		index = LTouchstick;
	else
		index = RTouchstick;
	
	direction = Touchsticks[index].StickPos - Touchsticks[index].Center;
	distance = direction.Size();
	if(distance >= Touchsticks[index].Radius)
		distance = 1.0f;
	else
		distance = distance / Touchsticks[index].Radius;
	direction = direction.GetSafeNormal(0.001f);
	direction *= distance;
	
	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Touchstick[%i]:(%f, %f)"), index, direction.X, direction.Y));
	
	return direction;
}

// Set a callback for a button.
void ANNPPlayerController::SetButtonAction(NNPButtons button, void *object, ButtonAction callback)
{
	ButtonActions[button] = callback;
	CallingObject[button] = object;
}

// Get the current value of the requested button.
float ANNPPlayerController::GetButton(NNPButtons button)
{
	return Buttons[button];
}

void ANNPPlayerController::InitializeHaptics()
{
	// Get a pointer the the haptics engine.
	Haptics = [CHHapticEngine init];
	[Haptics startAndReturnError:nil];
	
	// Setup Haptics pattern.
	CHHapticEventParameter *intensity = [[CHHapticEventParameter alloc] initWithParameterID:CHHapticEventParameterIDHapticIntensity value:0.3f];
	CHHapticEventParameter *sharpness = [[CHHapticEventParameter alloc] initWithParameterID:CHHapticEventParameterIDHapticSharpness value:0.5f];
	NSArray *parameters = [NSArray arrayWithObjects:intensity, sharpness, nil];
	
	CHHapticEvent *hapticEvent = [[CHHapticEvent alloc] initWithEventType:CHHapticEventTypeHapticContinuous parameters:parameters relativeTime:0.0 duration:5.0];
	CHHapticPattern *continuousPattern = [[CHHapticPattern alloc] initWithEvents:[NSArray arrayWithObject:hapticEvent] parameters:[NSArray array] error:nil];
	
	id hapticPlayer = [Haptics createAdvancedPlayerWithPattern:continuousPattern error:nil];
	[hapticPlayer setCompletionHandler: ^(NSError *_Nullable error){
		// NNP TODO: handle upon completion...
	}];
}
