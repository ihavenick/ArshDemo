// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBarWidget.h"

#include "ArshDemoCharacter.h"
#include "Components/ProgressBar.h"



void UHealthBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	
	// HealthBar->PercentDelegate.BindUFunction(this, "GetHealthPercentage");
	// HealthBar->SynchronizeProperties();
}
