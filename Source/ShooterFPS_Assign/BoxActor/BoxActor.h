// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoxActor.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class UWidgetComponent;
class UUserWidget;

UCLASS()
class SHOOTERFPS_ASSIGN_API ABoxActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ABoxActor();
	/*virtual void Tick(float DeltaTime) override;*/

	void InitializeBox(int32 InHealth, int32 InScore, FColor InColor, const FVector& InLocation, const FRotator& InRotation, const FVector& InScale);

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor BoxColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Score;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
	UWidgetComponent* HitCountWidgetComponent;

	void UpdateHitWidget(int32 RemainingHits);  // Function to update the widget

	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UUserWidget> HitCountWidgetClass;

protected:
	virtual void BeginPlay() override;

private:
	/**
	*	Components
	*/
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* BoxMesh;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY()
	UUserWidget* HitCountWidgetInstance;
};
