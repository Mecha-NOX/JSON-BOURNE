// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UBoxComponent;
class UProjectileMovementComponent;

UCLASS()
class SHOOTERFPS_ASSIGN_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* CollisionBox;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;

	/** Tracer particle effect */
	UPROPERTY(VisibleAnywhere, Category = "Effects")
	UParticleSystemComponent* TracerEffect;

	/** Tracer Impact effect */
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* ImpactEffect;

	/** Tracer Impact Sound */
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* ImpactSound;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	int32 HitDamage = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float ProjectileSpeed = 100000.0f;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
