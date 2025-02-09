// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterFPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Engine/Engine.h"
#include "Components/CapsuleComponent.h"
#include "ShooterFPS_Assign/Weapon/Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h" 
#include "ShooterFPS_Assign/HUD/ShooterHUDWidget.h"
#include "TimerManager.h"

AShooterFPSCharacter::AShooterFPSCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(GetCapsuleComponent());
	FollowCamera->SetRelativeLocation(FVector(-15.f, -10.0f, 50.0f));

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->GravityScale = 1.0f;
	GetCharacterMovement()->JumpZVelocity = 420.0f;
	GetCharacterMovement()->AirControl = 0.05f;

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = true;
	bUseControllerRotationRoll = false;

	// Default lane is the middle lane
	CurrentLane = 1;
	bIsChangingLanes = false;

	if (FollowCamera)
	{
		DefaultFOV = FollowCamera->FieldOfView;
		ADSFOV = 35.f;

		DefaultCameraPosition = FollowCamera->GetRelativeLocation();
		ADSCameraPosition = FVector(-50.f, -2.0f, 50.0f);
	}

	bIsAiming = false;
	bIsFiring = false;

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(false);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);  // Ignore camera collisions

	PlayerScore = 0;
}

void AShooterFPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* ShooterFPSPlayerController = Cast<APlayerController>(Controller);
	if (ShooterFPSPlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ShooterFPSPlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	if (ShooterHUDWidgetClass)
	{
		ShooterHUDWidget = CreateWidget<UShooterHUDWidget>(GetWorld(), ShooterHUDWidgetClass);
		if (ShooterHUDWidget)
		{
			ShooterHUDWidget->AddToViewport();
		}
	}
	AttachWeapon();
	// Set initial position in the middle lane
	CurrentLane = 1;
	LaneTargetPosition = GetActorLocation();
	LaneTargetPosition.Y = 150 + (-1 + CurrentLane) * LaneOffset;
	SetActorLocation(LaneTargetPosition);
}

void AShooterFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Continuously update lane position smoothly
	UpdateLanePosition(DeltaTime);

	// Clamp X position after movement
	ClampXAxisMovement();

	if (FollowCamera)
	{
		// Interpolate FOV for smooth ADS transition
		float TargetFOV = bIsAiming ? ADSFOV : DefaultFOV;
		FollowCamera->SetFieldOfView(FMath::FInterpTo(FollowCamera->FieldOfView, TargetFOV, DeltaTime, 10.0f));

		// Interpolate Camera Position for smooth movement
		FVector TargetPosition = bIsAiming ? ADSCameraPosition : DefaultCameraPosition;
		FollowCamera->SetRelativeLocation(FMath::VInterpTo(FollowCamera->GetRelativeLocation(), TargetPosition, DeltaTime, 10.f));
	}

	if (FPSWeapon)
	{
		UpdateHUDAmmo(FPSWeapon->GetAmmoInClip());
	}
}

void AShooterFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &AShooterFPSCharacter::MoveForward);
		EnhancedInputComponent->BindAction(MoveLeftAction, ETriggerEvent::Triggered, this, &AShooterFPSCharacter::MoveLeft);
		EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &AShooterFPSCharacter::MoveRight);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShooterFPSCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AShooterFPSCharacter::Jump);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AShooterFPSCharacter::StartFiring);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AShooterFPSCharacter::StopFiring);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AShooterFPSCharacter::StartAiming);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AShooterFPSCharacter::StopAiming);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &AShooterFPSCharacter::StartReloading);
	}
}

void AShooterFPSCharacter::Jump()
{
	Super::Jump();
}

void AShooterFPSCharacter::UpdateHUDAmmo(int32 NewAmmo)
{
	if (ShooterHUDWidget)
	{
		ShooterHUDWidget->UpdateAmmo(NewAmmo);
	}
}

void AShooterFPSCharacter::UpdateHUDScore(int32 NewScore)
{
	if (ShooterHUDWidget)
	{
		ShooterHUDWidget->UpdateScore(NewScore);
		UE_LOG(LogTemp, Warning, TEXT("UPDATING HUD SCORE!!!"));
	}
}

void AShooterFPSCharacter::MoveForward(const FInputActionValue& Value)
{
	const float MoveValue = Value.Get<float>(); // Get Movement Value from the InputAction Value
	if (MoveValue != 0.f)
	{
		FVector ForwardDirection = GetActorForwardVector();
		//	Prevent movement along Y - axis(sideways)
		ForwardDirection.Y = 0.0f;
		//	Prevent movement along Z - axis
		ForwardDirection.Y = 0.0f;

		if (!ForwardDirection.IsNearlyZero()) // Prevent Crash
		{
			//	Normalize for consistent speed
			ForwardDirection.Normalize();
			AddMovementInput(ForwardDirection, MoveValue);
		}
	}
}

void AShooterFPSCharacter::ClampXAxisMovement()
{
	FVector NewLocation = GetActorLocation();
	NewLocation.X = FMath::Clamp(NewLocation.X, -2600.0f, -1600.f);
	SetActorLocation(NewLocation);
}

void AShooterFPSCharacter::MoveLeft(const FInputActionValue& Value)
{
	if (!bIsChangingLanes && CurrentLane > 0) // Prevent moving beyond the leftmost lane
	{
		ChangeLane(CurrentLane - 1);
	}
}

void AShooterFPSCharacter::MoveRight(const FInputActionValue& Value)
{
	if (!bIsChangingLanes && CurrentLane < 2) // Prevent moving beyond the rightmost lane
	{
		ChangeLane(CurrentLane + 1);
	}
}

void AShooterFPSCharacter::ChangeLane(int32 NewLane)
{
	CurrentLane = NewLane;
	LaneTargetPosition = GetActorLocation();
	LaneTargetPosition.Y = 150 + (-1 + CurrentLane) * LaneOffset;
	bIsChangingLanes = true;

	// Play lane switch sound
	if (LaneSwitchSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, LaneSwitchSound, GetActorLocation());
	}
}

void AShooterFPSCharacter::UpdateLanePosition(float DeltaTime)
{
	if (bIsChangingLanes)
	{
		// Smooth interpolation to target lane position
		FVector NewPosition = FMath::VInterpTo(GetActorLocation(), LaneTargetPosition, DeltaTime, 15.0f);
		SetActorLocation(NewPosition);

		if (FVector::DistSquared(GetActorLocation(), LaneTargetPosition) < 1.0f)
		{
			SetActorLocation(LaneTargetPosition);
			bIsChangingLanes = false;
		}
	}
}

void AShooterFPSCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	if (Controller)
	{
		FRotator NewRotation = Controller->GetControlRotation();

		float NewPitch = NewRotation.Pitch - (LookAxisValue.Y * SensitivityY);
		float NewYaw = NewRotation.Yaw + (LookAxisValue.X * SensitivityX);

		// Clamp the Pitch angle
		NewRotation.Pitch = FMath::Clamp(NewPitch, -55.0f, 45.0f);
		// Yaw Doesnt need Clamping
		NewRotation.Yaw = NewYaw;

		//Apply new rotation
		Controller->SetControlRotation(NewRotation);

		/*if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Yellow, FString::Printf(TEXT("NewRotation.Pitch: %f"), NewRotation.Pitch));
		}*/
	}
}

void AShooterFPSCharacter::StartAiming()
{
	bIsAiming = true;
}

void AShooterFPSCharacter::StopAiming()
{
	bIsAiming = false;
}

void AShooterFPSCharacter::FireWeapon()
{
	if (FPSWeapon && FPSWeapon->CanFire())
	{
		FPSWeapon->Fire();

		// Stop firing if out of ammo
		if (!FPSWeapon->CanFire())
		{
			StopFiring();
			StartReloading();
		}
	}
}

void AShooterFPSCharacter::StartFiring()
{
	if (FPSWeapon && FPSWeapon->CanFire())
	{
		bIsFiring = true;
		FireWeapon(); // Fire once immediately

		// Start continuous firing timer
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AShooterFPSCharacter::FireWeapon, FPSWeapon->GetFireRate(), true);
	}
}

void AShooterFPSCharacter::StopFiring()
{
	bIsFiring = false;
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void AShooterFPSCharacter::StartReloading()
{
	if (FPSWeapon)
	{
		FPSWeapon->Reload();
	}
}

void AShooterFPSCharacter::AttachWeapon()
{
	if (WeaponClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		// Spawn the weapon
		FPSWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		if (FPSWeapon)
		{
			// Attach weapon to character's mesh at GripPoint socket
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
			FPSWeapon->AttachToComponent(GetMesh(), AttachmentRules, TEXT("GripPoint"));
		}
	}
}