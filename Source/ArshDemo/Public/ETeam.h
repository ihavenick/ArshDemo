// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ETeam.generated.h"



UENUM(BlueprintType)
enum class ETeamsEnum : uint8
{
 
	ETeam_A            UMETA(DisplayName = "A Takımı"),
	ETeam_B            UMETA(DisplayName = "B Takımı"),
 
};
