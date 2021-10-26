// Copyright Epic Games, Inc. All Rights Reserved.

#include "ArshDemoGameMode.h"

#include <string>

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

	AArshDemoCharacter* Pawn = Cast<AArshDemoCharacter>(NewPlayer->GetPawn());

	if (!Pawn)
		UE_LOG(LogTemp, Warning, TEXT("Pawn bok yolu"));
	
	if (TeamA.Contains(NewPlayer->GetUniqueID()))
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

AActor* AArshDemoGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	
	// if (TeamA.Contains(Player->GetUniqueID()))
	// {
	// 	int32 Random = FMath::RandRange(0,TeamAStart.Num()-1);
	// 	return TeamAStart[Random];	
	// }
	// else if (TeamB.Contains(Player->GetUniqueID()))
	// {
	// 	int32 Random = FMath::RandRange(0,TeamBStart.Num()-1);
	// 	return TeamBStart[Random];
	// }
	// UE_LOG(LogTemp, Warning, TEXT("%d ID li oyuncuyu biryere sokamadık"),Player->GetUniqueID());
	return Super::ChoosePlayerStart_Implementation(Player);
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
			if (PlayerStart->PlayerStartTag.Compare(FName("TeamA")))
				TeamAStart.Add(PlayerStart);
			else if (PlayerStart->PlayerStartTag.Compare(FName("TeamB")))
				TeamBStart.Add(PlayerStart);
	}

	UE_LOG(LogTemp, Warning, TEXT("A takımı %d Spawn noktası var , B takımının %d spawn noktası var"),TeamAStart.Num(),TeamBStart.Num());
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
		int32 Random = FMath::RandRange(0,TeamAStart.Num()-1);
		return TeamAStart[Random];	
	}
	else if (TeamB.Contains(Player->GetUniqueID()))
	{
		int32 Random = FMath::RandRange(0,TeamBStart.Num()-1);
		return TeamBStart[Random];
	}
	UE_LOG(LogTemp, Warning, TEXT("%d ID li oyuncuyu biryere sokamadık"),Player->GetUniqueID());
	
	
	return Super::FindPlayerStart_Implementation(Player, IncomingName);
}
