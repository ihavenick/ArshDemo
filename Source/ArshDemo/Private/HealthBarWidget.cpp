// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBarWidget.h"

#include "ArshDemoCharacter.h"
#include "Components/ProgressBar.h"



void UHealthBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HealthBar->SetFillColorAndOpacity(FLinearColor::Red);
}

void UHealthBarWidget::SetColorGreen()
{
	if (HealthBar)
		HealthBar->SetFillColorAndOpacity(FLinearColor::Green);
}
