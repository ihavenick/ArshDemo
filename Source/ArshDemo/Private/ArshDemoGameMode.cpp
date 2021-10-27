// Copyright Epic Games, Inc. All Rights Reserved.

#include "ArshDemoGameMode.h"

#include "Engine.h"

#include "ArshDemoCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"



AArshDemoGameMode::AArshDemoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}


void AArshDemoGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AddPawnTeamInfo(NewPlayer);
}


void AArshDemoGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), FoundActors);


	for (AActor* FoundActor : FoundActors)
	{
		APlayerStart* PlayerStart = Cast<APlayerStart>(FoundActor);
		if (PlayerStart)
		{
			if (PlayerStart->PlayerStartTag == FName("TeamA"))
				TeamAStart.Push(PlayerStart);
			else if (PlayerStart->PlayerStartTag== FName("TeamB"))
				TeamBStart.Push(PlayerStart);
		}
			
	}

	UE_LOG(LogTemp, Warning, TEXT("A team has %d Spawnpoint , B team has %d spawnpoint"),TeamAStart.Num(),TeamBStart.Num());
}


AActor* AArshDemoGameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	if (!Player->ActorHasTag(FName("HasTeam")))
	{
		if (TeamA.Num() <= TeamB.Num())
		{
			TeamA.Add(Player->GetUniqueID());
			UE_LOG(LogTemp, Warning, TEXT("Adding player to Team A"));
		}
		else
		{
			TeamB.Add(Player->GetUniqueID());
			UE_LOG(LogTemp, Warning, TEXT("Adding player to Team B"));
		}
		Player->Tags.Add(FName("HasTeam"));
	}

	if (TeamA.Contains(Player->GetUniqueID()))
	{
		const int32 Random = FMath::RandRange(0,TeamAStart.Num()-1);
		return TeamAStart[Random];	
	}
	else if (TeamB.Contains(Player->GetUniqueID()))
	{
		const int32 Random = FMath::RandRange(0,TeamBStart.Num()-1);
		return TeamBStart[Random];
	}
	UE_LOG(LogTemp, Warning, TEXT("%d ID li oyuncuyu biryere sokamadık"),Player->GetUniqueID());
	
	
	return Super::FindPlayerStart_Implementation(Player, IncomingName);
}

void AArshDemoGameMode::RestartPlayerwithTimer(AController* Controller)
{
	UE_LOG(LogTemp, Warning, TEXT("%d ID li oyuncuyu canlandirma sirasina ekledik"),Controller->GetUniqueID());
	RestartPlayer(Controller);

	DeadPlayers.Add(Controller);
	GetWorldTimerManager().SetTimer(DeadTimer, this, &AArshDemoGameMode::Respawn, 4.f);
}

void AArshDemoGameMode::AddPawnTeamInfo(AController* Controller)
{
	AArshDemoCharacter* Pawn = Cast<AArshDemoCharacter>(Controller->GetPawn());
	
	if (TeamA.Contains(Controller->GetUniqueID()))
	{
		Pawn->Team = ETeamsEnum::ETeam_A;
		Pawn->Tags.Add(FName("TeamA"));
		UE_LOG(LogTemp, Warning, TEXT("Assinged Pawn to Team A"));
	}
	else
	{
		Pawn->Team = ETeamsEnum::ETeam_B;
		Pawn->Tags.Add(FName("TeamB"));
		UE_LOG(LogTemp, Warning, TEXT("Assinged Pawn to Team B"));
	}
}

void AArshDemoGameMode::Respawn()
{
	for (auto DeadPlayer : DeadPlayers)
	{
		AController* Controller = DeadPlayer;
		APawn* pawn = nullptr;
	
		if (TeamA.Contains(Controller->GetUniqueID()))
		{
			UE_LOG(LogTemp, Warning, TEXT("%d ID li oyuncuyu A takiminda canlandiriyoz"),Controller->GetUniqueID());
			const int32 Random = FMath::RandRange(0,TeamAStart.Num()-1);
			RestartPlayerAtPlayerStart(Controller ,TeamAStart[Random]);
			//pawn = SpawnDefaultPawnFor(Controller,TeamAStart[0]);
		}
		else if (TeamB.Contains(Controller->GetUniqueID()))
		{
			UE_LOG(LogTemp, Warning, TEXT("%d ID li oyuncuyu B takiminda canlandiriyoz"),Controller->GetUniqueID());
			const int32 Random = FMath::RandRange(0,TeamBStart.Num()-1);
			RestartPlayerAtPlayerStart(Controller ,TeamBStart[Random]);
			//pawn = SpawnDefaultPawnFor(Controller,TeamBStart[0]);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%d ID li oyuncuyu GG Takimsiz kaldi"),Controller->GetUniqueID());
		}

		AddPawnTeamInfo(Controller);
		DeadPlayers.Remove(DeadPlayer);
	}
	//Controller->Possess(pawn);
}
