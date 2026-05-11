#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JCRRaceCheckpoint.generated.h"

class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FJCRCheckpointPassed, class AJCRRaceCheckpoint*, Checkpoint);

UCLASS(Blueprintable)
class JCRRACERV4_API AJCRRaceCheckpoint : public AActor
{
    GENERATED_BODY()

public:
    AJCRRaceCheckpoint();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Checkpoint")
    TObjectPtr<UBoxComponent> Trigger;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
    int32 CheckpointIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
    bool bFinish = false;

    UPROPERTY(BlueprintAssignable, Category = "Checkpoint")
    FJCRCheckpointPassed OnCheckpointPassed;

protected:
    UFUNCTION()
    void HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
