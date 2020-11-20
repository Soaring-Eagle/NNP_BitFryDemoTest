// Copyright Epic Games, Inc. All Rights Reserved.

#include "NNP_BitFryTestDemoCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#define CAMERA_MOVE_SCALE 2.5f

void HandleButtonCallbacks(NNPButtons button, void *object, bool pressed)
{
	ANNP_BitFryTestDemoCharacter *character;
	
	if(!object)
		return;
	
	character = (ANNP_BitFryTestDemoCharacter*)object;
	character->HandleButtons(button, pressed);
}

//////////////////////////////////////////////////////////////////////////
// ANNP_BitFryTestDemoCharacter

ANNP_BitFryTestDemoCharacter::ANNP_BitFryTestDemoCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	
	// NNP: Initialize my demo controller.
	NNPController = CreateDefaultSubobject<ANNPPlayerController>(TEXT("NNPPlayerController"));
}

//////////////////////////////////////////////////////////////////////////
// Input

void ANNP_BitFryTestDemoCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	
	if(NNPController)
		NNPController->InitializeHardwareController(Controller->GetControlRotation());
		
	if(NNPController->IsInitialized())
	{
		NNPController->SetButtonAction(AButton, this, HandleButtonCallbacks);
		
		PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ANNP_BitFryTestDemoCharacter::DoNothing);
		PlayerInputComponent->BindAction("Jump", IE_Released, this, &ANNP_BitFryTestDemoCharacter::DoNothing);
	}
	else
	{
		PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
		PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	}

	PlayerInputComponent->BindAxis("MoveForward", this, &ANNP_BitFryTestDemoCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ANNP_BitFryTestDemoCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ANNP_BitFryTestDemoCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ANNP_BitFryTestDemoCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ANNP_BitFryTestDemoCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ANNP_BitFryTestDemoCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ANNP_BitFryTestDemoCharacter::OnResetVR);
}

void ANNP_BitFryTestDemoCharacter::HandleButtons(NNPButtons button, bool pressed)
{
	switch(button)
	{
		case AButton:
			if(pressed)
				Jump();
			else
				StopJumping();
			break;
			
		default:
			break;
	}
}

void ANNP_BitFryTestDemoCharacter::DoNothing()
{
	
}

void ANNP_BitFryTestDemoCharacter::OnResetVR()
{
	// If NNP_BitFryTestDemo is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in NNP_BitFryTestDemo.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ANNP_BitFryTestDemoCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ANNP_BitFryTestDemoCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ANNP_BitFryTestDemoCharacter::TurnAtRate(float Rate)
{
	float deltaSeconds = GetWorld()->GetDeltaSeconds();
	
	// calculate delta for this frame from the rate information
	if(NNPController && NNPController->IsInitialized())
	{
		FVector2D thumbstick = NNPController->GetThumbstick(false);
		NNPController->AddYawInput(thumbstick.X * BaseTurnRate * deltaSeconds * CAMERA_MOVE_SCALE);
		Controller->SetControlRotation(NNPController->GetOrientation());
	}
	else
		AddControllerYawInput(Rate * BaseTurnRate * deltaSeconds);
}

void ANNP_BitFryTestDemoCharacter::LookUpAtRate(float Rate)
{
	float deltaSeconds = GetWorld()->GetDeltaSeconds();
	
	// calculate delta for this frame from the rate information
	if(NNPController && NNPController->IsInitialized())
	{
		FVector2D thumbstick = NNPController->GetThumbstick(false);
		NNPController->AddPitchInput(-thumbstick.Y * BaseLookUpRate * deltaSeconds * CAMERA_MOVE_SCALE);
		Controller->SetControlRotation(NNPController->GetOrientation());
	}
	else
		AddControllerPitchInput(Rate * BaseLookUpRate * deltaSeconds);
}

void ANNP_BitFryTestDemoCharacter::MoveForward(float Value)
{
	if(NNPController && NNPController->IsInitialized())
	{
		// Find out which way is forward.
		FRotator rotation = NNPController->GetOrientation();
		FRotator yawRotation(0.0f, rotation.Yaw, 0.0f);
		
		// Get the value of the forward vector.
		FVector2D thumbstick = NNPController->GetThumbstick();
		float value = thumbstick.Y;
		
		// Get the forward vector.
		FVector direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(direction, value);
	}
	else if ((Controller != nullptr) && (Value != 0.0f))
	{
		//if(GEngine)
		//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Not Using NNPController!"));
		
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ANNP_BitFryTestDemoCharacter::MoveRight(float Value)
{
	if(NNPController && NNPController->IsInitialized())
	{
		// Find out which way is forward.
		FRotator rotation = NNPController->GetOrientation();
		FRotator yawRotation(0.0f, rotation.Yaw, 0.0f);
		
		// Get the value of the forward vector.
		FVector2D thumbstick = NNPController->GetThumbstick();
		float value = thumbstick.X;
		
		// Get the forward vector.
		FVector direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(direction, value);
	}
	else if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		//if(GEngine)
		//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Not Using NNPController!"));
		
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
