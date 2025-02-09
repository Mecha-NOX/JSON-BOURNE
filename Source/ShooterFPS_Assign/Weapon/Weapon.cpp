// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "ShooterFPS_Assign/Projectile/Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	SetRootComponent(WeaponMesh);

	// Disable all collisions for the weapon mesh
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	AmmoInClip = ClipSize;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::Fire()
{
	if (CanFire())
	{
		//Decrease Ammo
		AmmoInClip--;
		UE_LOG(LogTemp, Warning, TEXT("Weapon Fired! Ammo Left: %d"), AmmoInClip);

		// Fire Weapon Logic
		AActor* OwnerActor = GetOwner();
		if (!OwnerActor) return;

		//	Get Muzzle Transform
		FVector MuzzleLocation = WeaponMesh->GetSocketLocation(MuzzleSocketName);
		FRotator MuzzleRotation = WeaponMesh->GetSocketRotation(MuzzleSocketName);

		//	Spawn Projectile
		if (ProjectileClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = OwnerActor;
			SpawnParams.Instigator = OwnerActor->GetInstigator();
			GetWorld()->SpawnActor<AProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
		}

		// Play fire sound
		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, MuzzleLocation);
		}

		// Play Muzzle Flash Effect
		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAttached(
				MuzzleFlash,
				WeaponMesh,
				FName("MuzzleSocket"), // Socket where the particle should appear
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::SnapToTarget
			);
		}
	}	
}

void AWeapon::Reload()
{
	UE_LOG(LogTemp, Warning, TEXT("Reload Function Reached!"));
	if (CanReload())
	{
		bIsReloading = true;
		UE_LOG(LogTemp, Warning, TEXT("Reloading..."));

		// Play reload sound
		if (ReloadSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, GetActorLocation());
		}

		// Simulate reload time
		GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &AWeapon::FinishReload, ReloadTime, false);
	}
}

void AWeapon::FinishReload()
{
	GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
	AmmoInClip = ClipSize;
	bIsReloading = false;
	bCanFire = true;
	UE_LOG(LogTemp, Warning, TEXT("Reload Complete! Ammo Refilled: %d"), AmmoInClip);
}

bool AWeapon::CanFire()
{
	return AmmoInClip > 0 && !bIsReloading;
}

bool AWeapon::CanReload()
{
	UE_LOG(LogTemp, Warning, TEXT("Player Can Reload!"));
	return AmmoInClip < ClipSize && !bIsReloading;
}
