// Copyright Epic Games, Inc. All Rights Reserved.

#include "Tears_RecreationCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Rewindable.h"

//////////////////////////////////////////////////////////////////////////
// ATears_RecreationCharacter

ATears_RecreationCharacter::ATears_RecreationCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	physicsHandler = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandler"));
	physicsHandler->InterpolationSpeed = 10; 

	m_MaxGrabDistance = 1000;

}

//////////////////////////////////////////////////////////////////////////
// Input

void ATears_RecreationCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ATears_RecreationCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ATears_RecreationCharacter::MoveRight);

	PlayerInputComponent->BindAction("Toggle Rewind", IE_Pressed, this, &ATears_RecreationCharacter::ToggleRewindAbility);
	PlayerInputComponent->BindAction("Activate Ability", IE_Pressed, this, &ATears_RecreationCharacter::ActivateAbility);
	PlayerInputComponent->BindAction("Activate Ability", IE_Released, this, &ATears_RecreationCharacter::ReleaseAttached);
	PlayerInputComponent->BindAction("Toggle Attach", IE_Pressed, this, &ATears_RecreationCharacter::ToggleAttachAbility);
	//PlayerInputComponent->BindAction("Toggle Rewind", IE_Released, this, &ATears_RecreationCharacter::DestroyRewindHud);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &ATears_RecreationCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &ATears_RecreationCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ATears_RecreationCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ATears_RecreationCharacter::TouchStopped);
}

void ATears_RecreationCharacter::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
	if (m_RewindToggle)
	{
		if (LineTraceMethod(m_rewindHitResult))
		{
			AActor* hitActor = m_rewindHitResult.GetActor();
			if (Cast<ARewindable>(hitActor))
			{
				m_validTarget = true;
			}
			else
			{
				m_validTarget = false;
			}
		}
	}
	if (m_AttachToggle)
	{
		if (LineTraceMethod(m_attachHitResult))
		{
			AActor* hitRewind = m_attachHitResult.GetActor();
			if (ARewindable* rewindTemp = Cast<ARewindable>(hitRewind))
			{
				m_validTarget = true;
				if (rewindTemp->isGrabbed())
				{

					if (physicsHandler->GrabbedComponent)
					{

						physicsHandler->SetTargetLocationAndRotation(GetFollowCamera()->GetComponentLocation() + (GetFollowCamera()->GetForwardVector() * 800), rewindTemp->GetActorRotation());
						//rewindTemp->physicsHandler->SetTargetLocationAndRotation(temp_Location, temp_Rotation); 
						GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Magenta, FString::Printf(TEXT("I Am Grabbed")));
					}
					else 
					{
						FVector temp_Location = m_attachHitResult.GetComponent()->GetComponentLocation();
						FRotator temp_Rotation = m_attachHitResult.GetComponent()->GetComponentRotation();
						physicsHandler->GrabComponentAtLocationWithRotation(m_attachHitResult.GetComponent(),FName(TEXT("NONE")), temp_Location, temp_Rotation);
					}
				}
			}
			else
			{
				physicsHandler->ReleaseComponent();
			}

		}
	}

}

void ATears_RecreationCharacter::ReleaseAttached()
{
	if (physicsHandler->GrabbedComponent)
	{	
		if (ARewindable* tempRewind= Cast<ARewindable>(m_attachHitResult.GetActor())) 
		{
			tempRewind->SetGrabbed(false); 
		}
		physicsHandler->ReleaseComponent();
	}
}


void ATears_RecreationCharacter::ToggleRewindAbility()
{
	m_AttachToggle = false;
	if (m_RewindToggle)
	{
		m_RewindToggle = false;
		m_validTarget = false;
	}
	else
	{
		m_RewindToggle = true;
	}
	LerpCamera();
}
void ATears_RecreationCharacter::ToggleAttachAbility()
{
	m_RewindToggle = false;

	if (m_AttachToggle)
	{
		m_AttachToggle = false;
		m_validTarget = false;

	}
	else
	{
		m_AttachToggle = true;
	}
	LerpCamera();
}


void ATears_RecreationCharacter::CreateRewindHud()
{

}

void ATears_RecreationCharacter::DestroyRewindHud()
{
}

void ATears_RecreationCharacter::ActivateAbility()
{
	if (m_validTarget && m_RewindToggle)
	{
		ARewindable* temp_Rewind = Cast<ARewindable>(m_rewindHitResult.GetActor());
		if (temp_Rewind->GetRewind() == false)
		{
			temp_Rewind->SetRewind(true);
			temp_Rewind->StartRewind();
			m_RewindToggle = false;
			m_validTarget = false;
			LerpCamera();
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Magenta, FString::Printf(TEXT("It's rewind time")));
		}
	}
	if (m_validTarget && m_AttachToggle)
	{
		ARewindable* temp_Rewind = Cast<ARewindable>(m_attachHitResult.GetActor());
		if (temp_Rewind->isGrabbed() == false)
		{
			temp_Rewind->SetGrabbed(true);
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Magenta, FString::Printf(TEXT("I Am Clicked")));
		}
	}

}




bool ATears_RecreationCharacter::LineTraceMethod(FHitResult& outHit)
{
	FVector start = GetFollowCamera()->GetComponentLocation(); // gets the starting point of the camera
	FVector forward = GetFollowCamera()->GetForwardVector() * 1000; // gets the forward vector and multiplies it by 1000 to get the max range
	FVector end = start + forward;

	FCollisionQueryParams parameters;

	parameters.AddIgnoredActor(this); // makers it ignore itself for the hit

	DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 0.5f, 1.f, 0.5f);
	return GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECollisionChannel::ECC_Visibility, parameters);
}

void ATears_RecreationCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ATears_RecreationCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void ATears_RecreationCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ATears_RecreationCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}



void ATears_RecreationCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ATears_RecreationCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
