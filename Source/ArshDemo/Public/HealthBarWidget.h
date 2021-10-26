// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARSHDEMO_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar *HealthBar;
	virtual void NativeConstruct() override;
	
public:
	FORCEINLINE class UProgressBar *GetHealthBar() { return HealthBar; }
};
