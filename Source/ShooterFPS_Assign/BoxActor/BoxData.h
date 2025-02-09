#pragma once

#include "CoreMinimal.h"
//#include "UObject/NoExportTypes.h"
#include "BoxData.generated.h"

USTRUCT(BlueprintType)
struct FBoxType
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Box")
    FString Name;

    UPROPERTY(BlueprintReadWrite, Category = "Box")
    FColor Color;

    UPROPERTY(BlueprintReadWrite, Category = "Box")
    int32 Health;

    UPROPERTY(BlueprintReadWrite, Category = "Box")
    int32 Score;

    bool ParseFromJson(const TSharedPtr<FJsonObject>& JsonObject)
    {
        if (!JsonObject.IsValid()) return false;

        Name = JsonObject->GetStringField("name");
        Health = JsonObject->GetIntegerField("health");
        Score = JsonObject->GetIntegerField("score");

        // Parse Color (Array of 3 values)
        TArray<TSharedPtr<FJsonValue>> ColorArray = JsonObject->GetArrayField("color");
        if (ColorArray.Num() == 3)
        {
            Color = FColor(ColorArray[0]->AsNumber(), ColorArray[1]->AsNumber(), ColorArray[2]->AsNumber(), 255);
        }
        else
        {
            return false;
        }
        return true;
    }
};

USTRUCT(BlueprintType)
struct FBoxTransform
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Box")
    FVector Location;

    UPROPERTY(BlueprintReadWrite, Category = "Box")
    FRotator Rotation;

    UPROPERTY(BlueprintReadWrite, Category = "Box")
    FVector Scale;

    bool ParseFromJson(const TSharedPtr<FJsonObject>& JsonObject)
    {
        if (!JsonObject.IsValid()) return false;

        TArray<TSharedPtr<FJsonValue>> LocationArray = JsonObject->GetArrayField("location");
        TArray<TSharedPtr<FJsonValue>> RotationArray = JsonObject->GetArrayField("rotation");
        TArray<TSharedPtr<FJsonValue>> ScaleArray = JsonObject->GetArrayField("scale");

        if (LocationArray.Num() == 3 && RotationArray.Num() == 3 && ScaleArray.Num() == 3)
        {
            Location = FVector(LocationArray[0]->AsNumber(), LocationArray[1]->AsNumber(), LocationArray[2]->AsNumber());
            Rotation = FRotator(RotationArray[0]->AsNumber(), RotationArray[1]->AsNumber(), RotationArray[2]->AsNumber());
            Scale = FVector(ScaleArray[0]->AsNumber(), ScaleArray[1]->AsNumber(), ScaleArray[2]->AsNumber());
            return true;
        }
        return false;
    }
};

USTRUCT(BlueprintType)
struct FBoxObject
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Box")
    FString Type;

    UPROPERTY(BlueprintReadWrite, Category = "Box")
    FBoxTransform Transform;

    bool ParseFromJson(const TSharedPtr<FJsonObject>& JsonObject)
    {
        if (!JsonObject.IsValid()) return false;

        Type = JsonObject->GetStringField("type");

        // Parse Transform
        TSharedPtr<FJsonObject> TransformObject = JsonObject->GetObjectField("transform");
        if (TransformObject.IsValid())
        {
            return Transform.ParseFromJson(TransformObject);
        }
        return false;
    }
};

USTRUCT(BlueprintType)
struct FBoxData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Box")
    TArray<FBoxType> Types;

    UPROPERTY(BlueprintReadWrite, Category = "Box")
    TArray<FBoxObject> Objects;

    bool ParseFromJson(const FString& JsonString)
    {
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(JsonString);

        if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
        {
            return false;
        }

        // Parse Box Types
        const TArray<TSharedPtr<FJsonValue>>* TypesArray;
        if (JsonObject->TryGetArrayField("types", TypesArray))
        {
            for (const TSharedPtr<FJsonValue>& Value : *TypesArray)
            {
                TSharedPtr<FJsonObject> ObjectData = Value->AsObject();
                if (ObjectData.IsValid())
                {
                    FBoxType BoxType;
                    if (BoxType.ParseFromJson(ObjectData))
                    {
                        Types.Add(BoxType);
                    }
                }
            }
        }

        // Parse Box Objects
        const TArray<TSharedPtr<FJsonValue>>* ObjectsArray;
        if (JsonObject->TryGetArrayField("objects", ObjectsArray))
        {
            for (const TSharedPtr<FJsonValue>& Value : *ObjectsArray)
            {
                TSharedPtr<FJsonObject> ObjectData = Value->AsObject();
                if (ObjectData.IsValid())
                {
                    FBoxObject BoxObject;
                    if (BoxObject.ParseFromJson(ObjectData))
                    {
                        Objects.Add(BoxObject);
                    }
                }
            }
        }

        return true;
    }
};