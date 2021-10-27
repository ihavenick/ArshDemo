// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ETeam.h"
#include "GameFramework/Character.h"
#include "ArshDemoCharacter.generated.h"

class UHealthComponent;
class UWidgetComponent;
class TeamsEnum;

UCLASS(config=Game)
class AArshDemoCharacter : public ACharacter
{
	GENERATED_BODY()
	
	FTimerHandle FTH_Team;
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	AArshDemoCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	FRotator ControllRotation;
	
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AProjectileActor> ProjectileClass;


	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerShoot();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawnProjectile();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category ="CombatVariables")
	UAnimMontage* AttackMontage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealthComponent* HealthComp;
	
	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);
	

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	UFUNCTION(NetMulticast, Reliable)
    void MulticastPlayMontage();
	
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void Shoot();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	// End of APawn interface

public:
	UFUNCTION(BlueprintCallable)
	void SpawnProjectile();
	//virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Team")
	ETeamsEnum Team;
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite, Category="Health")
	bool bDead;

	UFUNCTION(BlueprintCallable)
	void GetControllerRotationReplicated();
	
	UFUNCTION(Category = "Health")
	float GetHealthPercent();
	void OnHealthChanged(UHealthComponent* OwningHealthComp, float Health, float HealthDelta,
	                     const UDamageType* DamageType,
	                     AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void MakeEnemyBarsDifferent();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UWidgetComponent *HealthBar{nullptr};
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

