#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JCRRaceHUDWidget.generated.h"

class STextBlock;

UCLASS(Blueprintable)
class JCRRACERV4_API UJCRRaceHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;

private:
    TSharedPtr<STextBlock> SpeedText;
    TSharedPtr<STextBlock> RpmText;
    TSharedPtr<STextBlock> GearText;
    TSharedPtr<STextBlock> TimerText;
    TSharedPtr<STextBlock> CheckpointText;
    TSharedPtr<STextBlock> AssistText;
    TSharedPtr<STextBlock> ModeText;
    TSharedPtr<STextBlock> MiniMapText;
};
