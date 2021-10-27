// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

#include "ArshDemoCharacter.h"
#include "ArshDemoGameMode.h"
#include "ProjectileActor.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	DefaultHealth = 100;
	bIsDead = false;
	
	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();


	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);
		}
	}

	Health = DefaultHealth;
}

void UHealthComponent::OnRep_Health(float OldHealth)
{
	float Damage = Health - OldHealth;

	OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}


void UHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy,
	AActor* DamageCauser)
{
	if (Damage <= 0.0f || bIsDead)
	{
		return;
	}
	AArshDemoCharacter* PawnDamaged = Cast<AArshDemoCharacter>(DamagedActor);
	AArshDemoCharacter* PawnDamager = Cast<AArshDemoCharacter>(DamageCauser);

	if (!PawnDamaged)
	return;
	if (!PawnDamager)
	return;
	if (DamageCauser == DamagedActor || PawnDamaged->Team == PawnDamager->Team)
		return;

	// Update health clamped
	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

	bIsDead = Health <= 0.0f;

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

	if (bIsDead)
	{
		AArshDemoGameMode* GM = Cast<AArshDemoGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->RestartPlayerwithTimer(PawnDamaged->GetController());
		}
	}
}


float UHealthComponent::GetHealth() const
{
	return Health;
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, Health);
}

