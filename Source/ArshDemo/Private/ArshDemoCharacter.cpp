// Copyright Epic Games, Inc. All Rights Reserved.

#include "ArshDemoCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "HealthBarWidget.h"
#include "HealthComponent.h"
#include "ProjectileActor.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/ProgressBar.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

//////////////////////////////////////////////////////////////////////////
// AArshDemoCharacter

AArshDemoCharacter::AArshDemoCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	bDead = false;
	
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	HealthBar = CreateDefaultSubobject<UWidgetComponent>(FName("HealthBar"));
	static ConstructorHelpers::FClassFinder<UUserWidget> MenuWidgetClassFinder(TEXT("/Game/Core/UI/UI_HealthBar"));
	HealthBar->SetWidgetClass(MenuWidgetClassFinder.Class);
	HealthBar->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	HealthBar->SetRelativeTransform(FTransform(FQuat(0,0,0,0),FVector(0,0,100),FVector(0,0.235f,0.0325f)));

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));
	
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

    TargetPoint = CreateDefaultSubobject<UArrowComponent>(FName("TargetArrow"));
	TargetPoint->AttachTo(GetMesh(),FName("ShootLocation"),EAttachLocation::SnapToTarget,false);
	TargetPoint->SetHiddenInGame(true);
    TargetPoint->SetRelativeRotation(FRotator(90,0,0),false,nullptr);
	
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	AttackMontage = LoadObject<UAnimMontage>(AttackMontage,TEXT("/Game/Mannequin/Animations/AttackAnim.AttackAnim"));
	
	//HealthBarWidget->create
	bReplicates = true;
}

void AArshDemoCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &AArshDemoCharacter::Shoot);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AArshDemoCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AArshDemoCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AArshDemoCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AArshDemoCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AArshDemoCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AArshDemoCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AArshDemoCharacter::OnResetVR);
}

void AArshDemoCharacter::ServerShoot_Implementation()
{
	MulticastPlayMontage();
}

bool AArshDemoCharacter::ServerShoot_Validate()
{
	return true;
}

void AArshDemoCharacter::SpawnProjectile()
{
	if (HasAuthority())
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			FRotator SpawnRotation;
			FVector SpawnLocation;;
			
			TargetPoint->GetSocketWorldLocationAndRotation("",SpawnLocation,SpawnRotation);
		
			SpawnRotation = GetFollowCamera()->GetComponentRotation();
			//SpawnLocation.X += 100;
			//SpawnLocation.Z += 60;
			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			if (const auto Projectile = World->SpawnActor<AProjectileActor>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams))
				Projectile->SetDamagerOwner(this);
		}
	}

	
}

void AArshDemoCharacter::ServerSpawnProjectile_Implementation()
{
	SpawnProjectile();
}

bool AArshDemoCharacter::ServerSpawnProjectile_Validate()
{
	return true;
}

void AArshDemoCharacter::Shoot()
{
	if (!HasAuthority())
    {
	    ServerShoot();
    }
	
	if(!GetCurrentMontage())
	{
		PlayAnimMontage(AttackMontage,1);
	}
	
}

void AArshDemoCharacter::MulticastPlayMontage_Implementation()
{
	PlayAnimMontage(AttackMontage,1);
}

void AArshDemoCharacter::BeginPlay()
{
	Super::BeginPlay();

	
	HealthBar->InitWidget();
	const auto HealBarUserWidget = Cast<UHealthBarWidget>(HealthBar->GetUserWidgetObject());
	if (HealBarUserWidget)
	{
		HealBarUserWidget->GetHealthBar()->PercentDelegate.BindUFunction(this, FName("GetHealthPercent"));
	}
	
	//GetWorldTimerManager().SetTimer(FTH_Team, this, &AArshDemoCharacter::MakeEnemyBarsDifferent, 4.f);

	HealthComp->OnHealthChanged.AddUniqueDynamic(this, &AArshDemoCharacter::OnHealthChanged);
	
}

void AArshDemoCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(HealthBar)
	{
		FRotator CameraRotation = UGameplayStatics::GetPlayerCameraManager(GetWorld(),0)->GetCameraRotation();
		CameraRotation.Yaw-=180;
		HealthBar->SetWorldRotation(CameraRotation);
	}
	GetControllerRotationReplicated();
}

float AArshDemoCharacter::GetHealthPercent() const
{
	return HealthComp->GetHealth() / 100;
}

void AArshDemoCharacter::Ragdoll_Implementation()
{
	USkeletalMeshComponent* mesh = GetMesh();
	UCharacterMovementComponent* cm = GetCharacterMovement();
	UCapsuleComponent* UCC = GetCapsuleComponent();

	UCC->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	cm->SetMovementMode(MOVE_None);

	mesh->SetCollisionObjectType(ECC_PhysicsBody);

	mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	mesh->SetAllBodiesSimulatePhysics(true);


	TArray<UStaticMeshComponent*> Components;
	
	GetComponents<UStaticMeshComponent>(Components);
	for( int32 i=0; i<Components.Num(); i++ )
	{
		UStaticMeshComponent* StaticMeshComponent = Components[i];
		StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}


void AArshDemoCharacter::OnHealthChanged(UHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType,
	class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bDead)
	{
		bDead = true;
		
		GetMovementComponent()->StopMovementImmediately();
		//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//DetachFromControllerPendingDestroy();
		
		Ragdoll();
		
		//SetLifeSpan(10.0f);
	}
}



void AArshDemoCharacter::MakeEnemyBarsDifferent()
{
	//GetWorldTimerManager().ClearTimer(FTH_Team);

	TArray<AActor*> Characters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), StaticClass(),Characters);

	for (AActor* FoundActor : Characters)
	{
		AArshDemoCharacter* Pawn = Cast<AArshDemoCharacter>(FoundActor);
		if (Pawn)
		{
			if (Team==Pawn->Team)
			{
				auto OthersBar = Cast<UHealthBarWidget>(Pawn->HealthBar->GetUserWidgetObject());
				if (OthersBar)
					OthersBar->SetColorGreen();
			}
			
		}
	}
}

void AArshDemoCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArshDemoCharacter, Team);
	//DOREPLIFETIME(AArshDemoCharacter, ControllRotation);
	DOREPLIFETIME_CONDITION(AArshDemoCharacter, ControllRotation, COND_SkipOwner);
	DOREPLIFETIME(AArshDemoCharacter, bDead);
}



void AArshDemoCharacter::OnResetVR()
{
	// If ArshDemo is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in ArshDemo.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AArshDemoCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AArshDemoCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}


void AArshDemoCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AArshDemoCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AArshDemoCharacter::GetControllerRotationReplicated()
{
	if (HasAuthority()||IsLocallyControlled())
	{
		ControllRotation = GetControlRotation();
	}
}

void AArshDemoCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AArshDemoCharacter::MoveRight(float Value)
{
	MakeEnemyBarsDifferent();
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
