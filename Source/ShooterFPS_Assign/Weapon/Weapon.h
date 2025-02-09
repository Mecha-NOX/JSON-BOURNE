// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class USkeletalMeshComponent;
class AProjectile;
class USoundBase;

UCLASS()
class SHOOTERFPS_ASSIGN_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;

	// Fires a bullet
	void Fire();

	// Starts the reload process
	void Reload();

	bool CanFire();
	bool CanReload();

protected:
	virtual void BeginPlay() override;

private:
	/**
	*	Components
	*/
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;

	// Projectile class to spawn
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AProjectile> ProjectileClass;

	// Sound to play when firing
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USoundBase* FireSound;

	// Sound to play when reloading
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USoundBase* ReloadSound;

	// Muzzle Flash Particle 
	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName MuzzleSocketName = "MuzzleSocket";

	/** Flags for Fire and reload Weapon states */
	bool bCanFire = true;	// To prevent Fire during other action states
	bool bIsReloading = false;

	// Handles the reload delay
	FTimerHandle ReloadTimerHandle;

	// Ammo properties
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int32 ClipSize = 30;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	int32 AmmoInClip;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float FireRate = 0.13f; // Adjust fire rate here

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float ReloadTime = 0.9f;

	// Function called after reloading finishes
	void FinishReload();

public:
	/** Getters for weapon stats */
	FORCEINLINE float GetFireRate() const { return FireRate; }
	FORCEINLINE int32 GetAmmoInClip() const { return AmmoInClip; }
};
