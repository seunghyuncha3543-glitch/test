#include "JCRRaceHUDWidget.h"

#include "JCRRaceManager.h"
#include "JCRVehiclePawn.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

TSharedRef<SWidget> UJCRRaceHUDWidget::RebuildWidget()
{
    return SNew(SBorder)
        .Padding(FMargin(22.0f))
        .BorderBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.28f))
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight()
            [
                SAssignNew(SpeedText, STextBlock)
                .Text(FText::FromString(TEXT("000 km/h")))
                .ColorAndOpacity(FSlateColor(FLinearColor::White))
            ]
            + SVerticalBox::Slot().AutoHeight()
            [
                SAssignNew(RpmText, STextBlock)
                .Text(FText::FromString(TEXT("RPM 0000")))
                .ColorAndOpacity(FSlateColor(FLinearColor(0.85f, 0.95f, 1.0f)))
            ]
            + SVerticalBox::Slot().AutoHeight()
            [
                SAssignNew(GearText, STextBlock)
                .Text(FText::FromString(TEXT("GEAR 1")))
                .ColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.92f, 0.72f)))
            ]
            + SVerticalBox::Slot().AutoHeight().Padding(0.0f, 12.0f, 0.0f, 0.0f)
            [
                SAssignNew(TimerText, STextBlock)
                .Text(FText::FromString(TEXT("00:00.000")))
                .ColorAndOpacity(FSlateColor(FLinearColor::White))
            ]
            + SVerticalBox::Slot().AutoHeight()
            [
                SAssignNew(CheckpointText, STextBlock)
                .Text(FText::FromString(TEXT("CP 0/0")))
                .ColorAndOpacity(FSlateColor(FLinearColor(0.75f, 1.0f, 0.82f)))
            ]
            + SVerticalBox::Slot().AutoHeight().Padding(0.0f, 12.0f, 0.0f, 0.0f)
            [
                SAssignNew(AssistText, STextBlock)
                .Text(FText::FromString(TEXT("ABS ON | TCS ON")))
                .ColorAndOpacity(FSlateColor(FLinearColor(0.78f, 0.9f, 1.0f)))
            ]
            + SVerticalBox::Slot().AutoHeight()
            [
                SAssignNew(ModeText, STextBlock)
                .Text(FText::FromString(TEXT("SPORT OFF | CAM 1")))
                .ColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.86f, 0.55f)))
            ]
            + SVerticalBox::Slot().AutoHeight().Padding(0.0f, 12.0f, 0.0f, 0.0f)
            [
                SAssignNew(MiniMapText, STextBlock)
                .Text(FText::FromString(TEXT("MAP: TOWN > STRAIGHT > MOUNTAIN > DESERT")))
                .ColorAndOpacity(FSlateColor(FLinearColor(0.72f, 0.84f, 1.0f)))
            ]
        ];
}

void UJCRRaceHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    const AJCRVehiclePawn* Vehicle = Cast<AJCRVehiclePawn>(UGameplayStatics::GetPlayerPawn(this, 0));
    const AJCRRaceManager* RaceManager = Cast<AJCRRaceManager>(UGameplayStatics::GetActorOfClass(this, AJCRRaceManager::StaticClass()));

    if (Vehicle)
    {
        SpeedText->SetText(FText::FromString(FString::Printf(TEXT("%03.0f km/h"), Vehicle->GetSpeedKmh())));
        RpmText->SetText(FText::FromString(FString::Printf(TEXT("RPM %04.0f"), Vehicle->GetDisplayRpm())));
        GearText->SetText(FText::FromString(FString::Printf(TEXT("GEAR %d"), Vehicle->GetDisplayGear())));
        AssistText->SetText(FText::FromString(FString::Printf(TEXT("ABS %s | TCS %s | BRK %.0f%%"),
            Vehicle->IsAbsEnabled() ? TEXT("ON") : TEXT("OFF"),
            Vehicle->IsTcsEnabled() ? TEXT("ON") : TEXT("OFF"),
            Vehicle->GetBrakeTemperature() * 100.0f)));
        ModeText->SetText(FText::FromString(FString::Printf(TEXT("SPORT %s | HB %s | CAM %d"),
            Vehicle->IsSportModeEnabled() ? TEXT("ON") : TEXT("OFF"),
            Vehicle->IsHandbrakeActive() ? TEXT("ON") : TEXT("OFF"),
            Vehicle->GetCameraMode() + 1)));
    }

    if (RaceManager)
    {
        const float Time = RaceManager->GetRaceTime();
        const int32 Minutes = FMath::FloorToInt(Time / 60.0f);
        const float Seconds = FMath::Fmod(Time, 60.0f);
        TimerText->SetText(FText::FromString(FString::Printf(TEXT("%02d:%06.3f"), Minutes, Seconds)));
        CheckpointText->SetText(FText::FromString(FString::Printf(TEXT("CP %d/%d%s"), RaceManager->GetNextCheckpointIndex(), RaceManager->GetCheckpointCount(), RaceManager->IsFinished() ? TEXT(" FINISH") : TEXT(""))));
    }
}
