#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "JCRGameMode.generated.h"

UCLASS()
class JCRRACERV4_API AJCRGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AJCRGameMode();
    virtual void BeginPlay() override;
};
