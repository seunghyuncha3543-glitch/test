#include "JCRRaceManager.h"

#include "JCRRaceCheckpoint.h"
#include "Kismet/GameplayStatics.h"

AJCRRaceManager::AJCRRaceManager()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AJCRRaceManager::BeginPlay()
{
    Super::BeginPlay();
    DiscoverCheckpoints();
}

void AJCRRaceManager::DiscoverCheckpoints()
{
    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(this, AJCRRaceCheckpoint::StaticClass(), Found);
    for (AActor* Actor : Found)
    {
        RegisterCheckpoint(Cast<AJCRRaceCheckpoint>(Actor));
    }

    Checkpoints.Sort([](const AJCRRaceCheckpoint& A, const AJCRRaceCheckpoint& B)
    {
        return A.CheckpointIndex < B.CheckpointIndex;
    });
}

void AJCRRaceManager::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!bFinished)
    {
        RaceTime += DeltaSeconds;
    }

    if (Checkpoints.Num() == 0)
    {
        DiscoverCheckpoints();
    }
}

void AJCRRaceManager::RegisterCheckpoint(AJCRRaceCheckpoint* Checkpoint)
{
    if (!Checkpoint || Checkpoints.Contains(Checkpoint))
    {
        return;
    }

    Checkpoints.Add(Checkpoint);
    Checkpoint->OnCheckpointPassed.AddUniqueDynamic(this, &AJCRRaceManager::HandleCheckpointPassed);
}

void AJCRRaceManager::HandleCheckpointPassed(AJCRRaceCheckpoint* Checkpoint)
{
    if (!Checkpoint || bFinished || Checkpoint->CheckpointIndex != NextCheckpointIndex)
    {
        return;
    }

    ++NextCheckpointIndex;
    if (Checkpoint->bFinish)
    {
        bFinished = true;
    }
}
