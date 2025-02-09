// Fill out your copyright notice in the Description page of Project Settings.


#include "BoxActor.h"
#include "ShooterFPS_Assign/ShooterFPS_Assign.h"
#include "Components/StaticMeshComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "ShooterFPS_Assign/Character/ShooterFPSCharacter.h"

ABoxActor::ABoxActor()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMesh"));
	SetRootComponent(BoxMesh);
		
	BoxMesh->SetCollisionObjectType(ECC_BoxMesh);
	BoxMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// Create the Widget Component
	HitCountWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HitCountWidget"));
	HitCountWidgetComponent->SetupAttachment(RootComponent);

	// Set it to World Space
	HitCountWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	HitCountWidgetComponent->SetDrawSize(FVector2D(200, 50));
	HitCountWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCountWidgetComponent->SetVisibility(false); // Hide by default
}

void ABoxActor::BeginPlay()
{
	Super::BeginPlay();

	if (HitCountWidgetClass)
	{
		// Create the widget and assign it
		HitCountWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), HitCountWidgetClass);
		if (HitCountWidgetInstance)
		{
			HitCountWidgetComponent->SetWidget(HitCountWidgetInstance);
			HitCountWidgetComponent->SetVisibility(true);
			UpdateHitWidget(Health); // Initial update to display correct value
		}
	}
}

void ABoxActor::UpdateHitWidget(int32 RemainingHits)
{
	if (!HitCountWidgetInstance) return; // Ensure widget exists

	UTextBlock* HitCountText = Cast<UTextBlock>(HitCountWidgetInstance->GetWidgetFromName(TEXT("HitCountText")));
	if (HitCountText)
	{
		HitCountText->SetText(FText::AsNumber(RemainingHits));
	}
}

//void ABoxActor::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

void ABoxActor::InitializeBox(int32 InHealth, int32 InScore, FColor InColor, const FVector& InLocation, const FRotator& InRotation, const FVector& InScale)
{
	Health = InHealth;
	Score = InScore;

	SetActorLocation(InLocation);
	SetActorRotation(InRotation);
	SetActorScale3D(InScale);

	UMaterialInterface* DefaultMaterial = BoxMesh->GetMaterial(0);
	if (DefaultMaterial)
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(DefaultMaterial, this);
		if (DynamicMaterial)
		{
			DynamicMaterial->SetVectorParameterValue("BaseColor", InColor);
			BoxMesh->SetMaterial(0, DynamicMaterial);
		}
	}

	// widget updates with the correct health value
	UpdateHitWidget(Health);
}

float ABoxActor::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const int32 DamageInt = FMath::RoundToInt(DamageAmount);
	Health -= DamageInt;
	UpdateHitWidget(Health);

	UE_LOG(LogTemp, Log, TEXT("BoxActor took %d damage. Remaining Health: %d"), DamageInt, Health);

	if (Health <= 0)
	{
		UE_LOG(LogTemp, Log, TEXT("BoxActor Destroyed!"));
		Destroy();
		// Increase Score
		if (EventInstigator)
		{
			AShooterFPSCharacter* ShootingCharacter = Cast<AShooterFPSCharacter>(EventInstigator->GetPawn());
			if (ShootingCharacter)
			{
				ShootingCharacter->PlayerScore += Score;
				UE_LOG(LogTemp, Log, TEXT("ShootingCharacter->PlayerScore: %d"), ShootingCharacter->PlayerScore);
				ShootingCharacter->UpdateHUDScore(ShootingCharacter->PlayerScore);
			}
		}
	}

	return DamageAmount;
}