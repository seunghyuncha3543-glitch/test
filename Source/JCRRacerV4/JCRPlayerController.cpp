#include "JCRPlayerController.h"

#include "JCRRaceHUDWidget.h"

void AJCRPlayerController::BeginPlay()
{
    Super::BeginPlay();

    SetInputMode(FInputModeGameOnly());
    bShowMouseCursor = false;

    TSubclassOf<UJCRRaceHUDWidget> ClassToCreate = HudWidgetClass;
    if (!ClassToCreate)
    {
        ClassToCreate = UJCRRaceHUDWidget::StaticClass();
    }
    HudWidget = CreateWidget<UJCRRaceHUDWidget>(this, ClassToCreate);
    if (HudWidget)
    {
        HudWidget->AddToViewport();
    }
}
