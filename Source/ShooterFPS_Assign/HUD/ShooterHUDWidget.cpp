// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterHUDWidget.h"
#include "Components/TextBlock.h"

void UShooterHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Initialize text values (optional)
    if (CurrentAmmo) CurrentAmmo->SetText(FText::FromString("0"));
    if (ScoreAmount) ScoreAmount->SetText(FText::FromString("0"));
}

void UShooterHUDWidget::UpdateAmmo(int32 AmmoCount)
{
    if (CurrentAmmo)
    {
        CurrentAmmo->SetText(FText::FromString(FString::Printf(TEXT("%d"), AmmoCount)));
    }
}

void UShooterHUDWidget::UpdateScore(int32 Score)
{
    if (ScoreAmount)
    {
        ScoreAmount->SetText(FText::FromString(FString::Printf(TEXT("%d"), Score)));
    }
}
