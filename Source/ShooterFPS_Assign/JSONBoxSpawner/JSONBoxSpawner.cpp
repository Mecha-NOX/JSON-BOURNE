// Fill out your copyright notice in the Description page of Project Settings.


#include "JSONBoxSpawner.h"

#include "Engine/World.h"
#include "JsonObjectConverter.h"

#include "Materials/MaterialInstanceDynamic.h"

AJSONBoxSpawner::AJSONBoxSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AJSONBoxSpawner::FetchJSONData()
{
    FString URL = TEXT("https://raw.githubusercontent.com/CyrusCHAU/Varadise-Technical-Test/refs/heads/main/data.json");

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
 
    UE_LOG(LogTemp, Warning, TEXT("This Pointer: %p"), this);
    Request->OnProcessRequestComplete().BindUObject(this, &AJSONBoxSpawner::OnResponseReceived);

    if (!Request->OnProcessRequestComplete().IsBound()) {
        UE_LOG(LogTemp, Error, TEXT("OnResponseReceived not bound!!"));
    }

    Request->SetURL(URL);
    Request->SetVerb("GET");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    if (!Request->ProcessRequest())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to process HTTP request."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("HTTP request sent successfully."));
    }
}

void AJSONBoxSpawner::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful || !Response.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to fetch JSON data. WasSuccessful: %d, Response Valid: %d"),
            bWasSuccessful, Response.IsValid() ? 1 : 0);
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("HTTP Response Code: %d"), Response->GetResponseCode());

    if (Response->GetResponseCode() != 200)
    {
        UE_LOG(LogTemp, Error, TEXT("HTTP request failed with status code: %d"), Response->GetResponseCode());
        return;
    }

    FString JsonString = Response->GetContentAsString();
    FBoxData BoxData;

    if (BoxData.ParseFromJson(JsonString))
    {
        UE_LOG(LogTemp, Log, TEXT("Successfully fetched and parsed JSON data."));

        for (const FBoxType& BoxType : BoxData.Types)
        {
            UE_LOG(LogTemp, Log, TEXT("Type: %s, Health: %d, Score: %d, Color: R:%d G:%d B:%d"),
                *BoxType.Name, BoxType.Health, BoxType.Score,
                BoxType.Color.R, BoxType.Color.G, BoxType.Color.B);
        }

        for (const FBoxObject& BoxObject : BoxData.Objects)
        {
            UE_LOG(LogTemp, Log, TEXT("Object Type: %s, Location: %s, Rotation: %s, Scale: %s"),
                *BoxObject.Type,
                *BoxObject.Transform.Location.ToString(),
                *BoxObject.Transform.Rotation.ToString(),
                *BoxObject.Transform.Scale.ToString());
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON data into struct."));
    }
    SpawnBoxesFromJSON(BoxData);
}

void AJSONBoxSpawner::BeginPlay()
{
	Super::BeginPlay();

    FetchJSONData();
}

void AJSONBoxSpawner::SpawnBoxesFromJSON(const FBoxData& BoxData)
{
    if (!BoxActorClass) // Ensure class is set
    {
        UE_LOG(LogTemp, Error, TEXT("BoxActorClass is not set in JSONBoxSpawner."));
        return;
    }

    for (const FBoxObject& BoxObject : BoxData.Objects)
    {
        for (const FBoxType& BoxType : BoxData.Types)
        {
            if (BoxObject.Type == BoxType.Name)
            {
                // Spawn Transform
                FTransform SpawnTransform(BoxObject.Transform.Rotation, BoxObject.Transform.Location, BoxObject.Transform.Scale);

                // Spawn BoxActor
                ABoxActor* BoxActor = GetWorld()->SpawnActor<ABoxActor>(BoxActorClass, SpawnTransform);

                if (BoxActor)
                {
                    // Initialize properties
                    BoxActor->InitializeBox(
                        BoxType.Health,
                        BoxType.Score,
                        BoxType.Color,
                        BoxObject.Transform.Location,
                        BoxObject.Transform.Rotation,
                        BoxObject.Transform.Scale
                    );
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("Failed to spawn BoxActor."));
                }
            }
        }
    }
}
