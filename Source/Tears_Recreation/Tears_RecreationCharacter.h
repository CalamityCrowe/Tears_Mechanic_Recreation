// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Tears_RecreationCharacter.generated.h"

UCLASS(config = Game)
class ATears_RecreationCharacter : public ACharacter
{
	GENERATED_BODY()

		/** Camera boom positioning the camera behind the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;
public:
	ATears_RecreationCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
		float TurnRateGamepad;


	void Tick(float deltaTime) override;
protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	// handles the inputs for rotating the camera or object held by the player
	virtual void AddControllerYawInput(float Rate) override;
	virtual void AddControllerPitchInput(float Rate) override;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
	UFUNCTION(BlueprintCallable, Category = "Rewind Ability")
		void ToggleRewindAbility();

	UFUNCTION(BlueprintCallable, Category = "Ultrahand")
		void EnableObjectRotation();
	UFUNCTION(BlueprintCallable, Category = "Ultrahand")
		void DisableObjectRotation();

	UFUNCTION(BlueprintCallable)
		bool LineTraceMethod(FHitResult& outHit);

	void ActivateAbility();

	void ReleaseAttached();

	UFUNCTION(BlueprintCallable)
		void ToggleAttachAbility();

	// an event that is called in code but bound in the blueprints
	UFUNCTION(BlueprintImplementableEvent)
		void LerpCamera();

	void MoveGrabbedObject(float rate);
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rewind Ability")
		bool m_RewindToggle;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ultrahand")
		bool m_AttachToggle;





	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ultrahand")
		bool m_RotateGrabbed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability Stats")
		bool m_validTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability Stats")
		float m_MaxGrabDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physics Component")
		class UPhysicsHandleComponent* physicsHandler;

	FHitResult m_rewindHitResult;
	FHitResult m_attachHitResult;
};

