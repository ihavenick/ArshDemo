// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArshDemoCharacter.h"
#include "GameFramework/Actor.h"
#include "ProjectileActor.generated.h"

class USoundCue;
class UProjectileMovementComponent;
class USphereComponent;
UCLASS()
class ARSHDEMO_API AProjectileActor : public AActor
{

	UPROPERTY(Replicated)
	AArshDemoCharacter* DamageCauser;
	
	GENERATED_BODY()
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	UParticleSystemComponent* Fireball;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	USoundCue* ImpactSFX;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;
protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	


public:	
	// Sets default values for this actor's properties
	AProjectileActor();
	
	UFUNCTION()
	void SetDamagerOwner(AArshDemoCharacter* Pawn);
	UFUNCTION()
	AArshDemoCharacter* GetDamagerOwner();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	UParticleSystem* FireParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	UParticleSystem* ImpactVFX;
	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

};
