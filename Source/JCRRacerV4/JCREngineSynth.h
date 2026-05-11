#pragma once

#include "CoreMinimal.h"
#include "Components/SynthComponent.h"
#include "JCREngineSynth.generated.h"

UCLASS(ClassGroup = Synth, meta = (BlueprintSpawnableComponent))
class JCRRACERV4_API UJCREngineSynth : public USynthComponent
{
    GENERATED_BODY()

public:
    UJCREngineSynth(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void SetDrivingState(float InSpeedKmh, float InThrottle, float InBrake, bool bInGrounded);

protected:
    virtual bool Init(int32& SampleRate) override;
    virtual int32 OnGenerateAudio(float* OutAudio, int32 NumSamples) override;

private:
    float EnginePhase = 0.0f;
    float ExhaustPhase = 0.0f;
    float NoiseState = 0.0f;
    float SmoothedRpm = 1100.0f;
    float SmoothedThrottle = 0.0f;
    float LastRawSample = 0.0f;
    float LastHighPassed = 0.0f;
    float CurrentSpeedKmh = 0.0f;
    float CurrentThrottle = 0.0f;
    float CurrentBrake = 0.0f;
    bool bGrounded = false;
    int32 CachedSampleRate = 48000;
};
