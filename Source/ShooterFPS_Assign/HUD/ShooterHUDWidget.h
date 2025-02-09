// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShooterHUDWidget.generated.h"

class UTextBlock;

UCLASS()
class SHOOTERFPS_ASSIGN_API UShooterHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

    /** Updates the ammo count displayed on the HUD */
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateAmmo(int32 AmmoCount);

    /** Updates the score displayed on the HUD */
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateScore(int32 Score);

protected:
    /** References to UI Text fields */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* CurrentAmmo;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ScoreAmount;
};
