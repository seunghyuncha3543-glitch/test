#include "JCRRaceCheckpoint.h"

#include "Components/BoxComponent.h"
#include "JCRVehiclePawn.h"

AJCRRaceCheckpoint::AJCRRaceCheckpoint()
{
    PrimaryActorTick.bCanEverTick = false;

    Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
    SetRootComponent(Trigger);
    Trigger->SetBoxExtent(FVector(70.0f, 520.0f, 260.0f));
    Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
    Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    Trigger->SetHiddenInGame(false);
    Trigger->ShapeColor = FColor::Cyan;
    Trigger->OnComponentBeginOverlap.AddDynamic(this, &AJCRRaceCheckpoint::HandleOverlap);
}

void AJCRRaceCheckpoint::HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (Cast<AJCRVehiclePawn>(OtherActor))
    {
        OnCheckpointPassed.Broadcast(this);
    }
}
