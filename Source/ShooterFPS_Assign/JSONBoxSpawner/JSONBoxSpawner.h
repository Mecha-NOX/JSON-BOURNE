// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Http.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h" 
#include "Interfaces/IHttpResponse.h"

#include "ShooterFPS_Assign/BoxActor/BoxData.h"
#include "ShooterFPS_Assign/BoxActor/BoxActor.h"

#include "JSONBoxSpawner.generated.h"

UCLASS()
class SHOOTERFPS_ASSIGN_API AJSONBoxSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AJSONBoxSpawner();

	void FetchJSONData();

	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<ABoxActor> BoxActorClass; //	Box class to spawn

private:	
	void SpawnBoxesFromJSON(const FBoxData& BoxData);
};
