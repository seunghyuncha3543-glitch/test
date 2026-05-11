#include "JCREngineSynth.h"

UJCREngineSynth::UJCREngineSynth(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bAutoActivate = true;
}

void UJCREngineSynth::SetDrivingState(float InSpeedKmh, float InThrottle, float InBrake, bool bInGrounded)
{
    CurrentSpeedKmh = FMath::Clamp(InSpeedKmh, 0.0f, 320.0f);
    CurrentThrottle = FMath::Clamp(InThrottle, 0.0f, 1.0f);
    CurrentBrake = FMath::Clamp(InBrake, 0.0f, 1.0f);
    bGrounded = bInGrounded;
}

bool UJCREngineSynth::Init(int32& SampleRate)
{
    CachedSampleRate = SampleRate;
    EnginePhase = 0.0f;
    ExhaustPhase = 0.0f;
    NoiseState = 0.0f;
    SmoothedRpm = 1100.0f;
    SmoothedThrottle = 0.0f;
    LastRawSample = 0.0f;
    LastHighPassed = 0.0f;
    return true;
}

int32 UJCREngineSynth::OnGenerateAudio(float* OutAudio, int32 NumSamples)
{
    const int32 NumFrames = NumSamples / 2;
    const float SpeedAlpha = FMath::Clamp(CurrentSpeedKmh / 260.0f, 0.0f, 1.0f);
    const float TargetRpm = FMath::Lerp(1500.0f, 7800.0f, FMath::Clamp(SpeedAlpha * 0.74f + CurrentThrottle * 0.38f, 0.0f, 1.0f));
    const float SmoothRate = 1.0f - FMath::Exp(-10.5f / FMath::Max(1, CachedSampleRate));
    const float EngineGain = FMath::Lerp(0.010f, 0.050f, CurrentThrottle) + SpeedAlpha * 0.012f;
    const float RoadGain = bGrounded ? SpeedAlpha * 0.012f : 0.0f;
    const float BrakeGain = bGrounded ? CurrentBrake * SpeedAlpha * 0.018f : 0.0f;

    for (int32 Frame = 0; Frame < NumFrames; ++Frame)
    {
        SmoothedRpm = FMath::Lerp(SmoothedRpm, TargetRpm, SmoothRate);
        SmoothedThrottle = FMath::Lerp(SmoothedThrottle, CurrentThrottle, SmoothRate * 1.45f);

        const float FiringHz = (SmoothedRpm / 60.0f) * 4.0f;
        const float ExhaustHz = FiringHz * 0.75f;
        EnginePhase = FMath::Fmod(EnginePhase + FiringHz / CachedSampleRate, 1.0f);
        ExhaustPhase = FMath::Fmod(ExhaustPhase + ExhaustHz / CachedSampleRate, 1.0f);

        const float Fundamental = FMath::Sin(EnginePhase * UE_TWO_PI) * 0.12f;
        const float HarmonicA = FMath::Sin(EnginePhase * UE_TWO_PI * 1.5f) * 0.34f;
        const float HarmonicB = FMath::Sin(EnginePhase * UE_TWO_PI * 2.5f) * 0.30f;
        const float HarmonicC = FMath::Sin(EnginePhase * UE_TWO_PI * 4.0f) * 0.18f;
        const float ExhaustPulse = FMath::Sin(ExhaustPhase * UE_TWO_PI) * FMath::Lerp(0.02f, 0.06f, SmoothedThrottle);

        NoiseState = FMath::Frac(NoiseState * 17.31f + 0.173f);
        const float Noise = (NoiseState * 2.0f - 1.0f);
        const float Engine = (Fundamental + HarmonicA + HarmonicB + HarmonicC + ExhaustPulse) * EngineGain;
        const float Road = Noise * RoadGain;
        const float Brake = FMath::Sin(EnginePhase * UE_TWO_PI * 7.0f) * BrakeGain + Noise * BrakeGain * 0.35f;
        const float RawSample = Engine + Road + Brake;
        const float HighPassed = 0.965f * (LastHighPassed + RawSample - LastRawSample);
        LastRawSample = RawSample;
        LastHighPassed = HighPassed;
        const float Sample = FMath::Clamp(HighPassed, -0.22f, 0.22f);

        OutAudio[Frame * 2] = Sample;
        OutAudio[Frame * 2 + 1] = Sample;
    }

    return NumSamples;
}
