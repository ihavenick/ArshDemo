// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ETeam.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerStart.h"
#include "ArshDemoGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor, AController*, KillerController);

UCLASS(minimalapi)
class AArshDemoGameMode : public AGameModeBase
{
	GENERATED_BODY()

private:
	TArray<uint32> TeamA;
	TArray<uint32> TeamB;

	TArray<APlayerStart*> TeamAStart;
	TArray<APlayerStart*> TeamBStart;

protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void BeginPlay() override;
	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;

public:
	AArshDemoGameMode();


	UPROPERTY(BlueprintAssignable, Category = "GameMode")
	FOnActorKilled OnActorKilled;
};



