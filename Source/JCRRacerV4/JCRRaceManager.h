#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JCRRaceManager.generated.h"

class AJCRRaceCheckpoint;

UCLASS(Blueprintable)
class JCRRACERV4_API AJCRRaceManager : public AActor
{
    GENERATED_BODY()

public:
    AJCRRaceManager();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintCallable, Category = "Race")
    void RegisterCheckpoint(AJCRRaceCheckpoint* Checkpoint);

    UFUNCTION(BlueprintPure, Category = "Race")
    float GetRaceTime() const { return RaceTime; }

    UFUNCTION(BlueprintPure, Category = "Race")
    int32 GetNextCheckpointIndex() const { return NextCheckpointIndex; }

    UFUNCTION(BlueprintPure, Category = "Race")
    int32 GetCheckpointCount() const { return Checkpoints.Num(); }

    UFUNCTION(BlueprintPure, Category = "Race")
    bool IsFinished() const { return bFinished; }

protected:
    UFUNCTION()
    void HandleCheckpointPassed(AJCRRaceCheckpoint* Checkpoint);

private:
    void DiscoverCheckpoints();

    UPROPERTY()
    TArray<TObjectPtr<AJCRRaceCheckpoint>> Checkpoints;

    float RaceTime = 0.0f;
    int32 NextCheckpointIndex = 0;
    bool bFinished = false;
};
