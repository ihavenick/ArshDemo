// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnProjectile.h"

#include "ArshDemoCharacter.h"
#include "ProjectileActor.h"

void USpawnProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (AArshDemoCharacter* Pawn = Cast<AArshDemoCharacter>(MeshComp->GetOwner()))
	{
		Pawn->SpawnProjectile();
	}
}
