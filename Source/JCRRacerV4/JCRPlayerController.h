#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "JCRPlayerController.generated.h"

class UJCRRaceHUDWidget;

UCLASS()
class JCRRACERV4_API AJCRPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UJCRRaceHUDWidget> HudWidgetClass;

private:
    UPROPERTY()
    TObjectPtr<UJCRRaceHUDWidget> HudWidget;
};
