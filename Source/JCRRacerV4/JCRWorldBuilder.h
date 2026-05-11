#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JCRWorldBuilder.generated.h"

class UStaticMesh;
class UStaticMeshComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class AJCRRaceCheckpoint;

UCLASS(Blueprintable)
class JCRRACERV4_API AJCRWorldBuilder : public AActor
{
    GENERATED_BODY()

public:
    AJCRWorldBuilder();

    virtual void BeginPlay() override;
    virtual void OnConstruction(const FTransform& Transform) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Course")
    float RoadWidthCm = 680.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Course")
    bool bSpawnCheckpointsAtRuntime = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Course")
    bool bBuildPreviewInEditor = true;

    UFUNCTION(BlueprintCallable, Category = "Course")
    void SpawnRuntimeCheckpoints();

private:
    UPROPERTY()
    TArray<TObjectPtr<UStaticMeshComponent>> PreviewComponents;

    UPROPERTY()
    TObjectPtr<UStaticMesh> CubeMesh;

    UPROPERTY()
    TObjectPtr<UStaticMesh> SphereMesh;

    UPROPERTY()
    TObjectPtr<UStaticMesh> CylinderMesh;

    UPROPERTY()
    TObjectPtr<UStaticMesh> ConeMesh;

    UPROPERTY()
    TObjectPtr<UStaticMesh> TrackTileMesh;

    UPROPERTY()
    TObjectPtr<UMaterialInterface> RoadMaterial;

    UPROPERTY()
    TObjectPtr<UMaterialInterface> FieldMaterial;

    UPROPERTY()
    TObjectPtr<UMaterialInterface> MountainMaterial;

    UPROPERTY()
    TObjectPtr<UMaterialInterface> HouseMaterial;

    UPROPERTY()
    TObjectPtr<UMaterialInterface> AccentMaterial;

    UPROPERTY()
    TObjectPtr<UMaterialInterface> WaterMaterial;

    UPROPERTY()
    TObjectPtr<UMaterialInterface> ShoulderMaterial;

    UPROPERTY()
    TObjectPtr<UMaterialInterface> LineYellowMaterial;

    UPROPERTY()
    TObjectPtr<UMaterialInterface> LineWhiteMaterial;

    UPROPERTY()
    TObjectPtr<UMaterialInterface> RailMaterial;

    UPROPERTY()
    TObjectPtr<UMaterialInterface> RoofMaterial;

    UPROPERTY()
    TObjectPtr<UMaterialInterface> FoliageMaterial;

    UPROPERTY()
    TObjectPtr<UMaterialInterface> RockMaterial;

    UPROPERTY()
    TObjectPtr<UMaterialInterface> MountainFarMaterial;

    UPROPERTY()
    TObjectPtr<UMaterialInterface> DesertMaterial;

    TArray<FVector> CoursePoints() const;
    void RefreshMaterials();
    void RebuildPreview();
    void SpawnLightingAndAtmosphere();
    UMaterialInterface* CreateTintMaterial(const FName& Name, const FLinearColor& Color, float Roughness = 0.6f, float Metallic = 0.0f);
    UStaticMeshComponent* AddBox(const FString& Name, const FVector& Location, const FVector& Scale, const FRotator& Rotation, UMaterialInterface* Material, bool bCollision = true);
    UStaticMeshComponent* AddMesh(const FString& Name, UStaticMesh* Mesh, const FVector& Location, const FVector& Scale, const FRotator& Rotation, UMaterialInterface* Material, bool bCollision = true);
    void AddRoadSegment(int32 Index, const FVector& Start, const FVector& End);
    void AddRoadPaint(int32 Index, const FVector& Start, const FVector& End, const FVector& Right, float Yaw);
    void AddGuardRail(const FVector& Start, const FVector& End, const FVector& Right, float Yaw, int32 Index);
    void AddSceneryAlongSegment(int32 Index, const FVector& Start, const FVector& End, const FVector& Right, float Yaw);
    void AddUtilityPole(const FVector& Location, float Yaw);
    void AddHouseCluster(const FVector& Location, float Yaw);
    void AddToriiGate(const FVector& Location, float Yaw);
    void AddFinishGate(const FVector& Location, float Yaw);
    void AddShrub(const FVector& Location, float Scale, const FLinearColor& Color);
    void AddTree(const FVector& Location, float Scale);
    void AddCactus(const FVector& Location, float Scale);
    void AddRock(const FVector& Location, float Scale);
    void AddMountainRidge();
};
