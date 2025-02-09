// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "ShooterFPSCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class AWeapon;
class USoundBase;
class UShooterHUDWidget;

UCLASS()
class SHOOTERFPS_ASSIGN_API AShooterFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AShooterFPSCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** <ACharacter> */
	virtual void Jump() override;
	/** </ACharacter> */

	void UpdateHUDAmmo(int32 NewAmmo);
	void UpdateHUDScore(int32 NewScore);

	int32 PlayerScore;

protected:
	virtual void BeginPlay() override;

	/**
	*	Input Actions
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveForwardAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveLeftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveRightAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ReloadAction;

	/**
	*	Callbacks for Inputs
	*/
	void MoveForward(const FInputActionValue& Value);
	void MoveLeft(const FInputActionValue& Value);
	void MoveRight(const FInputActionValue& Value);
	void ChangeLane(int32 NewLane);
	void Look(const FInputActionValue& Value);

	// Function to start firing when button is pressed
	void StartFiring();

	// Function to stop firing when button is released
	void StopFiring();

	// Start reload process
	void StartReloading();

	// Functions to start/stop ADSing
	void StartAiming();
	void StopAiming();

private:
	/**
	*	Components
	*/
	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* LaneSwitchSound;

	UPROPERTY()
	AWeapon* FPSWeapon; // Pointer to spawned weapon

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AWeapon> WeaponClass;  // Weapon type to spawn. Assigned from BP

	// Mouse Sensitivity Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = "true"))
	float SensitivityX = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = "true"))
	float SensitivityY = 0.7f;

	void AttachWeapon();
	void UpdateLanePosition(float DeltaTime);
	void ClampXAxisMovement();

	/**
	*	Lane control variables
	*/ 
	int32 CurrentLane = 1;             // 0 = Left, 1 = Middle, 2 = Right
	bool bIsChangingLanes;         // Track if currently moving between lanes
	FVector LaneTargetPosition;    // Target position for lane change
	const float LaneOffset = 350.f; // Distance between lanes

	// Fires weapon repeatedly while button is held
	void FireWeapon();

	// Flags for player state
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsFiring;

	bool bIsAiming;

	// Timer handle for continuous firing
	FTimerHandle FireTimerHandle;

	// ADS Variables
	float DefaultFOV;
	float ADSFOV;
	FVector DefaultCameraPosition;

	UPROPERTY(EditAnywhere, Category = "FOV", meta = (AllowPrivateAccess = "true"))
	FVector ADSCameraPosition;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> ShooterHUDWidgetClass;

	UPROPERTY()
	UShooterHUDWidget* ShooterHUDWidget;
};
