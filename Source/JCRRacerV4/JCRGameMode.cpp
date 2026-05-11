#include "JCRGameMode.h"

#include "JCRPlayerController.h"
#include "JCRRaceManager.h"
#include "JCRVehiclePawn.h"
#include "JCRWorldBuilder.h"
#include "Kismet/GameplayStatics.h"

AJCRGameMode::AJCRGameMode()
{
    DefaultPawnClass = AJCRVehiclePawn::StaticClass();
    PlayerControllerClass = AJCRPlayerController::StaticClass();
}

void AJCRGameMode::BeginPlay()
{
    Super::BeginPlay();

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    if (!UGameplayStatics::GetActorOfClass(this, AJCRWorldBuilder::StaticClass()))
    {
        AJCRWorldBuilder* Builder = GetWorld()->SpawnActor<AJCRWorldBuilder>(AJCRWorldBuilder::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, Params);
        if (Builder)
        {
#if WITH_EDITOR
            Builder->SetActorLabel(TEXT("Runtime_JapanCountryRoad"));
#endif
        }
    }

    if (!UGameplayStatics::GetActorOfClass(this, AJCRRaceManager::StaticClass()))
    {
        GetWorld()->SpawnActor<AJCRRaceManager>(AJCRRaceManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, Params);
    }
}
