#include "JCRWorldBuilder.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/LightComponent.h"
#include "Components/SkyLightComponent.h"
#include "JCRRaceCheckpoint.h"
#include "Engine/DirectionalLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/SkyLight.h"
#include "Materials/MaterialInterface.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/UObjectIterator.h"
#include "UObject/ConstructorHelpers.h"

AJCRWorldBuilder::AJCRWorldBuilder()
{
    PrimaryActorTick.bCanEverTick = false;
    SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeFinder.Succeeded())
    {
        CubeMesh = CubeFinder.Object;
    }

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereFinder(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderFinder(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeFinder(TEXT("/Engine/BasicShapes/Cone.Cone"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> TrackTileFinder(TEXT("/Game/VehicleTemplate/Meshes/SM_Track_10M.SM_Track_10M"));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> TrackMaterialFinder(TEXT("/Game/VehicleTemplate/Materials/M_Track.M_Track"));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> FieldMatFinder(TEXT("/Game/Environment/Materials/MI_JCR_RiceField.MI_JCR_RiceField"));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> WaterMatFinder(TEXT("/Game/Environment/Materials/MI_JCR_Water.MI_JCR_Water"));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> ShoulderMatFinder(TEXT("/Game/VehicleTemplate/Materials/MI_TrackEdge_Inst.MI_TrackEdge_Inst"));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> YellowMatFinder(TEXT("/Game/Environment/Materials/MI_JCR_YellowLine.MI_JCR_YellowLine"));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> WhiteMatFinder(TEXT("/Game/Environment/Materials/MI_JCR_WhiteLine.MI_JCR_WhiteLine"));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> RailMatFinder(TEXT("/Game/Environment/Materials/MI_JCR_Rail.MI_JCR_Rail"));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> HouseMatFinder(TEXT("/Game/Environment/Materials/MI_JCR_HouseWall.MI_JCR_HouseWall"));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> RoofMatFinder(TEXT("/Game/Environment/Materials/MI_JCR_Roof.MI_JCR_Roof"));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> ToriiMatFinder(TEXT("/Game/Environment/Materials/MI_JCR_Torii.MI_JCR_Torii"));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> FoliageMatFinder(TEXT("/Game/Environment/Materials/MI_JCR_Foliage.MI_JCR_Foliage"));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> RockMatFinder(TEXT("/Game/Environment/Materials/MI_JCR_Rock.MI_JCR_Rock"));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> MountainMatFinder(TEXT("/Game/Environment/Materials/MI_JCR_Mountain.MI_JCR_Mountain"));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> DesertMatFinder(TEXT("/Game/Environment/Materials/MI_JCR_DesertSand.MI_JCR_DesertSand"));
    SphereMesh = SphereFinder.Object;
    CylinderMesh = CylinderFinder.Object;
    ConeMesh = ConeFinder.Object;
    TrackTileMesh = TrackTileFinder.Object;

    UMaterialInterface* DefaultMaterial = UMaterial::GetDefaultMaterial(MD_Surface);
    if (TrackMaterialFinder.Succeeded())
    {
        RoadMaterial = TrackMaterialFinder.Object;
    }
    else
    {
        RoadMaterial = DefaultMaterial;
    }
    FieldMaterial = FieldMatFinder.Succeeded() ? FieldMatFinder.Object.Get() : DefaultMaterial;
    WaterMaterial = WaterMatFinder.Succeeded() ? WaterMatFinder.Object.Get() : DefaultMaterial;
    ShoulderMaterial = ShoulderMatFinder.Succeeded() ? ShoulderMatFinder.Object.Get() : DefaultMaterial;
    LineYellowMaterial = YellowMatFinder.Succeeded() ? YellowMatFinder.Object.Get() : DefaultMaterial;
    LineWhiteMaterial = WhiteMatFinder.Succeeded() ? WhiteMatFinder.Object.Get() : DefaultMaterial;
    RailMaterial = RailMatFinder.Succeeded() ? RailMatFinder.Object.Get() : DefaultMaterial;
    MountainMaterial = MountainMatFinder.Succeeded() ? MountainMatFinder.Object.Get() : DefaultMaterial;
    MountainFarMaterial = MountainMaterial;
    HouseMaterial = HouseMatFinder.Succeeded() ? HouseMatFinder.Object.Get() : DefaultMaterial;
    RoofMaterial = RoofMatFinder.Succeeded() ? RoofMatFinder.Object.Get() : DefaultMaterial;
    AccentMaterial = ToriiMatFinder.Succeeded() ? ToriiMatFinder.Object.Get() : DefaultMaterial;
    FoliageMaterial = FoliageMatFinder.Succeeded() ? FoliageMatFinder.Object.Get() : DefaultMaterial;
    RockMaterial = RockMatFinder.Succeeded() ? RockMatFinder.Object.Get() : DefaultMaterial;
    DesertMaterial = DesertMatFinder.Succeeded() ? DesertMatFinder.Object.Get() : ShoulderMaterial.Get();
}

void AJCRWorldBuilder::BeginPlay()
{
    Super::BeginPlay();
    RefreshMaterials();
    if (bSpawnCheckpointsAtRuntime)
    {
        SpawnRuntimeCheckpoints();
    }
    SpawnLightingAndAtmosphere();
}

void AJCRWorldBuilder::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    if (bBuildPreviewInEditor)
    {
        RefreshMaterials();
        RebuildPreview();
    }
}

TArray<FVector> AJCRWorldBuilder::CoursePoints() const
{
    return {
        FVector(0.0f, 0.0f, 0.0f),
        FVector(26000.0f, 0.0f, 0.0f),
        FVector(64000.0f, 1800.0f, 40.0f),
        FVector(90000.0f, 11800.0f, 180.0f),
        FVector(112000.0f, 26000.0f, 520.0f),
        FVector(136000.0f, 27000.0f, 820.0f),
        FVector(164000.0f, 15000.0f, 960.0f),
        FVector(192000.0f, 11000.0f, 720.0f),
        FVector(220000.0f, 3000.0f, 300.0f)
    };
}

void AJCRWorldBuilder::RefreshMaterials()
{
    auto LoadMat = [](const TCHAR* Path, UMaterialInterface* Fallback) -> UMaterialInterface*
    {
        if (UMaterialInterface* Loaded = LoadObject<UMaterialInterface>(nullptr, Path))
        {
            return Loaded;
        }
        return Fallback ? Fallback : UMaterial::GetDefaultMaterial(MD_Surface);
    };

    UMaterialInterface* DefaultMaterial = UMaterial::GetDefaultMaterial(MD_Surface);
    RoadMaterial = LoadMat(TEXT("/Game/VehicleTemplate/Materials/M_Track.M_Track"), RoadMaterial ? RoadMaterial.Get() : DefaultMaterial);
    FieldMaterial = LoadMat(TEXT("/Game/Environment/Materials/MI_JCR_RiceField.MI_JCR_RiceField"), FieldMaterial ? FieldMaterial.Get() : DefaultMaterial);
    WaterMaterial = LoadMat(TEXT("/Game/Environment/Materials/MI_JCR_Water.MI_JCR_Water"), WaterMaterial ? WaterMaterial.Get() : DefaultMaterial);
    ShoulderMaterial = LoadMat(TEXT("/Game/VehicleTemplate/Materials/MI_TrackEdge_Inst.MI_TrackEdge_Inst"), ShoulderMaterial ? ShoulderMaterial.Get() : DefaultMaterial);
    LineYellowMaterial = LoadMat(TEXT("/Game/Environment/Materials/MI_JCR_YellowLine.MI_JCR_YellowLine"), LineYellowMaterial ? LineYellowMaterial.Get() : DefaultMaterial);
    LineWhiteMaterial = LoadMat(TEXT("/Game/Environment/Materials/MI_JCR_WhiteLine.MI_JCR_WhiteLine"), LineWhiteMaterial ? LineWhiteMaterial.Get() : DefaultMaterial);
    RailMaterial = LoadMat(TEXT("/Game/Environment/Materials/MI_JCR_Rail.MI_JCR_Rail"), RailMaterial ? RailMaterial.Get() : DefaultMaterial);
    HouseMaterial = LoadMat(TEXT("/Game/Environment/Materials/MI_JCR_HouseWall.MI_JCR_HouseWall"), HouseMaterial ? HouseMaterial.Get() : DefaultMaterial);
    RoofMaterial = LoadMat(TEXT("/Game/Environment/Materials/MI_JCR_Roof.MI_JCR_Roof"), RoofMaterial ? RoofMaterial.Get() : DefaultMaterial);
    AccentMaterial = LoadMat(TEXT("/Game/Environment/Materials/MI_JCR_Torii.MI_JCR_Torii"), AccentMaterial ? AccentMaterial.Get() : DefaultMaterial);
    FoliageMaterial = LoadMat(TEXT("/Game/Environment/Materials/MI_JCR_Foliage.MI_JCR_Foliage"), FoliageMaterial ? FoliageMaterial.Get() : DefaultMaterial);
    RockMaterial = LoadMat(TEXT("/Game/Environment/Materials/MI_JCR_Rock.MI_JCR_Rock"), RockMaterial ? RockMaterial.Get() : DefaultMaterial);
    MountainFarMaterial = LoadMat(TEXT("/Game/Environment/Materials/MI_JCR_Mountain.MI_JCR_Mountain"), MountainFarMaterial ? MountainFarMaterial.Get() : DefaultMaterial);
    DesertMaterial = LoadMat(TEXT("/Game/Environment/Materials/MI_JCR_DesertSand.MI_JCR_DesertSand"), DesertMaterial ? DesertMaterial.Get() : ShoulderMaterial.Get());
    MountainMaterial = MountainFarMaterial;
}

void AJCRWorldBuilder::RebuildPreview()
{
    for (UStaticMeshComponent* Component : PreviewComponents)
    {
        if (Component)
        {
            Component->DestroyComponent();
        }
    }
    PreviewComponents.Reset();

    const TArray<FVector> Points = CoursePoints();
    for (int32 Index = 0; Index < Points.Num() - 1; ++Index)
    {
        AddRoadSegment(Index, Points[Index], Points[Index + 1]);
    }

    AddHouseCluster(FVector(1400.0f, -1150.0f, 120.0f), 5.0f);
    AddHouseCluster(FVector(4200.0f, 1100.0f, 120.0f), -8.0f);
    AddHouseCluster(FVector(8500.0f, -1450.0f, 105.0f), 2.0f);
    AddFinishGate(FVector(220000.0f, 3000.0f, 900.0f), -17.0f);

    AddBox(TEXT("OpenWorld_ValleyMeadow"), FVector(42000.0f, -7600.0f, -55.0f), FVector(850.0f, 95.0f, 0.10f), FRotator::ZeroRotator, FieldMaterial);
    AddBox(TEXT("OpenWorld_DesertFlat"), FVector(174000.0f, -12500.0f, 180.0f), FVector(900.0f, 150.0f, 0.10f), FRotator(0.0f, -9.0f, 0.0f), DesertMaterial);
    AddBox(TEXT("OpenWorld_DirtBypass"), FVector(176000.0f, -4600.0f, 300.0f), FVector(560.0f, 2.1f, 0.11f), FRotator(0.0f, -16.0f, 0.0f), DesertMaterial);
    AddBox(TEXT("OpenWorld_RockyPlain_A"), FVector(108000.0f, -13500.0f, 210.0f), FVector(620.0f, 86.0f, 0.09f), FRotator(0.0f, 13.0f, 0.0f), ShoulderMaterial);
    AddBox(TEXT("OpenWorld_RockyPlain_B"), FVector(137000.0f, 41000.0f, 620.0f), FVector(720.0f, 92.0f, 0.09f), FRotator(0.0f, -18.0f, 0.0f), FieldMaterial);
    AddBox(TEXT("OpenWorld_GravelLot"), FVector(6200.0f, 3400.0f, 0.0f), FVector(120.0f, 55.0f, 0.10f), FRotator(0.0f, 4.0f, 0.0f), ShoulderMaterial);
    AddBox(TEXT("WaterRetentionPond"), FVector(41000.0f, 7600.0f, -25.0f), FVector(220.0f, 36.0f, 0.025f), FRotator::ZeroRotator, WaterMaterial, false);
    for (int32 Cactus = 0; Cactus < 18; ++Cactus)
    {
        AddCactus(FVector(152000.0f + Cactus * 3600.0f, -11500.0f + FMath::Sin(Cactus * 0.9f) * 3600.0f, 390.0f), 0.75f + 0.05f * (Cactus % 5));
    }
    AddMountainRidge();
}

void AJCRWorldBuilder::SpawnLightingAndAtmosphere()
{
    if (!GetWorld())
    {
        return;
    }

    if (!FindObject<AActor>(GetWorld(), TEXT("JCR_DirectionalSun")))
    {
        ADirectionalLight* Sun = GetWorld()->SpawnActor<ADirectionalLight>(FVector(0.0f, 0.0f, 9000.0f), FRotator(-39.0f, -32.0f, 0.0f));
        if (Sun)
        {
            Sun->SetActorLabel(TEXT("JCR_DirectionalSun"));
            Sun->GetLightComponent()->Intensity = 8.5f;
            Sun->GetLightComponent()->LightColor = FColor(255, 244, 226);
            Sun->GetLightComponent()->bUseTemperature = true;
            Sun->GetLightComponent()->Temperature = 5200.0f;
        }
    }

    if (!FindObject<AActor>(GetWorld(), TEXT("JCR_SkyLight")))
    {
        ASkyLight* Sky = GetWorld()->SpawnActor<ASkyLight>(FVector::ZeroVector, FRotator::ZeroRotator);
        if (Sky)
        {
            Sky->SetActorLabel(TEXT("JCR_SkyLight"));
            Sky->GetLightComponent()->Intensity = 2.2f;
        }
    }

    if (!FindObject<AActor>(GetWorld(), TEXT("JCR_HeightFog")))
    {
        AExponentialHeightFog* Fog = GetWorld()->SpawnActor<AExponentialHeightFog>(FVector(0.0f, 0.0f, 250.0f), FRotator::ZeroRotator);
        if (Fog)
        {
            Fog->SetActorLabel(TEXT("JCR_HeightFog"));
            Fog->GetComponent()->FogDensity = 0.006f;
            Fog->GetComponent()->FogHeightFalloff = 0.12f;
            Fog->GetComponent()->SetFogInscatteringColor(FLinearColor(0.72f, 0.80f, 0.90f));
        }
    }

    if (!FindObject<AActor>(GetWorld(), TEXT("JCR_PostProcess")))
    {
        APostProcessVolume* Post = GetWorld()->SpawnActor<APostProcessVolume>(FVector::ZeroVector, FRotator::ZeroRotator);
        if (Post)
        {
            Post->SetActorLabel(TEXT("JCR_PostProcess"));
            Post->bUnbound = true;
            Post->Settings.bOverride_VignetteIntensity = true;
            Post->Settings.VignetteIntensity = 0.08f;
            Post->Settings.bOverride_BloomIntensity = true;
            Post->Settings.BloomIntensity = 0.16f;
            Post->Settings.bOverride_MotionBlurAmount = true;
            Post->Settings.MotionBlurAmount = 0.10f;
            Post->Settings.bOverride_ColorSaturation = true;
            Post->Settings.ColorSaturation = FVector4(1.14f, 1.10f, 1.04f, 1.0f);
            Post->Settings.bOverride_AutoExposureMinBrightness = true;
            Post->Settings.bOverride_AutoExposureMaxBrightness = true;
            Post->Settings.AutoExposureMinBrightness = 1.15f;
            Post->Settings.AutoExposureMaxBrightness = 1.85f;
        }
    }
}

UMaterialInterface* AJCRWorldBuilder::CreateTintMaterial(const FName& Name, const FLinearColor& Color, float Roughness, float Metallic)
{
    const FString Key = Name.ToString();
    if (Key.Contains(TEXT("Water")) || Key.Contains(TEXT("WetRice"))) { return WaterMaterial; }
    if (Key.Contains(TEXT("Shoulder"))) { return ShoulderMaterial; }
    if (Key.Contains(TEXT("Yellow"))) { return LineYellowMaterial; }
    if (Key.Contains(TEXT("White"))) { return LineWhiteMaterial; }
    if (Key.Contains(TEXT("Rail"))) { return RailMaterial; }
    if (Key.Contains(TEXT("Shrub"))) { return FoliageMaterial; }
    if (Key.Contains(TEXT("Rock"))) { return RockMaterial; }
    if (Key.Contains(TEXT("Mountain"))) { return MountainFarMaterial; }
    return AccentMaterial ? AccentMaterial.Get() : UMaterial::GetDefaultMaterial(MD_Surface);
}

UStaticMeshComponent* AJCRWorldBuilder::AddBox(const FString& Name, const FVector& Location, const FVector& Scale, const FRotator& Rotation, UMaterialInterface* Material, bool bCollision)
{
    return AddMesh(Name, CubeMesh, Location, Scale, Rotation, Material, bCollision);
}

UStaticMeshComponent* AJCRWorldBuilder::AddMesh(const FString& Name, UStaticMesh* Mesh, const FVector& Location, const FVector& Scale, const FRotator& Rotation, UMaterialInterface* Material, bool bCollision)
{
    const FName UniqueName = MakeUniqueObjectName(this, UStaticMeshComponent::StaticClass(), FName(*Name));
    UStaticMeshComponent* Component = NewObject<UStaticMeshComponent>(this, UniqueName);
    Component->SetStaticMesh(Mesh ? Mesh : CubeMesh.Get());
    Component->SetRelativeLocation(Location);
    Component->SetRelativeRotation(Rotation);
    Component->SetRelativeScale3D(Scale);
    Component->CreationMethod = EComponentCreationMethod::UserConstructionScript;
    Component->SetCollisionEnabled(bCollision ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
    Component->SetCollisionProfileName(bCollision ? TEXT("BlockAll") : TEXT("NoCollision"));
    Component->SetCastShadow(true);
    if (Material)
    {
        Component->SetMaterial(0, Material);
    }
    Component->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
    Component->RegisterComponent();
    PreviewComponents.Add(Component);
    return Component;
}

void AJCRWorldBuilder::AddRoadSegment(int32 Index, const FVector& Start, const FVector& End)
{
    const FVector Delta = End - Start;
    const float Length = Delta.Size2D();
    const FVector Mid = (Start + End) * 0.5f;
    const float Yaw = Delta.Rotation().Yaw;
    if (TrackTileMesh)
    {
        const FVector Forward = Delta.GetSafeNormal2D();
        const int32 TileCount = FMath::Max(1, FMath::CeilToInt(Length / 1000.0f));
        for (int32 Tile = 0; Tile < TileCount; ++Tile)
        {
            const float T = (Tile + 0.5f) / TileCount;
            const FVector TileLocation = FMath::Lerp(Start, End, T) + FVector(0.0f, 0.0f, 8.0f);
            const float TileScaleX = (Length / TileCount) / 1000.0f;
            AddMesh(FString::Printf(TEXT("TrackTile_%02d_%03d"), Index, Tile), TrackTileMesh, TileLocation, FVector(TileScaleX, 0.78f, 1.0f), FRotator(0.0f, Yaw, 0.0f), RoadMaterial);
        }
        UStaticMeshComponent* CollisionSlab = AddBox(FString::Printf(TEXT("RoadCollision_%02d"), Index), Mid + FVector(0.0f, 0.0f, -4.0f), FVector(Length / 100.0f, RoadWidthCm / 100.0f, 0.12f), FRotator(0.0f, Yaw, 0.0f), RoadMaterial);
        CollisionSlab->SetHiddenInGame(true);
    }
    else
    {
        AddBox(FString::Printf(TEXT("Road_%02d"), Index), Mid, FVector(Length / 100.0f, RoadWidthCm / 100.0f, 0.16f), FRotator(0.0f, Yaw, 0.0f), RoadMaterial);
    }

    const FVector Right = FRotationMatrix(FRotator(0.0f, Yaw, 0.0f)).GetScaledAxis(EAxis::Y);
    AddBox(FString::Printf(TEXT("Shoulder_L_%02d"), Index), Mid + Right * (RoadWidthCm * 0.62f), FVector(Length / 100.0f, 1.2f, 0.11f), FRotator(0.0f, Yaw, 0.0f), ShoulderMaterial);
    AddBox(FString::Printf(TEXT("Shoulder_R_%02d"), Index), Mid - Right * (RoadWidthCm * 0.62f), FVector(Length / 100.0f, 1.2f, 0.11f), FRotator(0.0f, Yaw, 0.0f), ShoulderMaterial);
    AddRoadPaint(Index, Start, End, Right, Yaw);
    AddGuardRail(Start, End, Right, Yaw, Index);
    AddSceneryAlongSegment(Index, Start, End, Right, Yaw);

    if (Index < 5)
    {
        for (int32 Pole = 0; Pole < 3; ++Pole)
        {
            const float T = (Pole + 1) / 4.0f;
            AddUtilityPole(FMath::Lerp(Start, End, T) + Right * 720.0f + FVector(0.0f, 0.0f, 260.0f), Yaw);
        }
    }

    if (Index == 2)
    {
        AddBox(TEXT("HighwaySign_SpeedTrap"), Mid + Right * 760.0f + FVector(0.0f, 0.0f, 360.0f), FVector(0.12f, 2.4f, 0.9f), FRotator(0.0f, Yaw, 0.0f), LineYellowMaterial, false);
    }
}

void AJCRWorldBuilder::AddRoadPaint(int32 Index, const FVector& Start, const FVector& End, const FVector& Right, float Yaw)
{
    const FVector Delta = End - Start;
    const float Length = Delta.Size2D();
    const FVector Mid = (Start + End) * 0.5f + FVector(0.0f, 0.0f, 13.0f);
    UMaterialInterface* Yellow = CreateTintMaterial(*FString::Printf(TEXT("MI_Yellow_%02d"), Index), FLinearColor(1.0f, 0.72f, 0.06f), 0.45f);
    UMaterialInterface* White = CreateTintMaterial(*FString::Printf(TEXT("MI_White_%02d"), Index), FLinearColor(0.86f, 0.88f, 0.83f), 0.42f);

    AddBox(FString::Printf(TEXT("CenterLine_%02d"), Index), Mid, FVector(Length / 100.0f, 0.035f, 0.018f), FRotator(0.0f, Yaw, 0.0f), Yellow, false);
    AddBox(FString::Printf(TEXT("EdgeLine_L_%02d"), Index), Mid + Right * (RoadWidthCm * 0.44f), FVector(Length / 100.0f, 0.028f, 0.018f), FRotator(0.0f, Yaw, 0.0f), White, false);
    AddBox(FString::Printf(TEXT("EdgeLine_R_%02d"), Index), Mid - Right * (RoadWidthCm * 0.44f), FVector(Length / 100.0f, 0.028f, 0.018f), FRotator(0.0f, Yaw, 0.0f), White, false);

    for (int32 Dash = 0; Dash < 5; ++Dash)
    {
        const float T = (Dash + 0.5f) / 5.0f;
        AddBox(FString::Printf(TEXT("LaneDash_%02d_%02d"), Index, Dash), FMath::Lerp(Start, End, T) + FVector(0.0f, 0.0f, 16.0f), FVector(2.0f, 0.055f, 0.02f), FRotator(0.0f, Yaw, 0.0f), Yellow, false);
    }
}

void AJCRWorldBuilder::AddGuardRail(const FVector& Start, const FVector& End, const FVector& Right, float Yaw, int32 Index)
{
    if (Index < 3)
    {
        return;
    }

    const FVector Mid = (Start + End) * 0.5f + FVector(0.0f, 0.0f, 92.0f);
    const float Length = (End - Start).Size2D();
    UMaterialInterface* RailMat = RailMaterial;
    AddBox(FString::Printf(TEXT("GuardRail_L_%02d"), Index), Mid + Right * 560.0f, FVector(Length / 100.0f, 0.08f, 0.08f), FRotator(0.0f, Yaw, 0.0f), RailMat);
    AddBox(FString::Printf(TEXT("GuardRail_R_%02d"), Index), Mid - Right * 560.0f, FVector(Length / 100.0f, 0.08f, 0.08f), FRotator(0.0f, Yaw, 0.0f), RailMat);
}

void AJCRWorldBuilder::AddSceneryAlongSegment(int32 Index, const FVector& Start, const FVector& End, const FVector& Right, float Yaw)
{
    const int32 Count = Index < 3 ? 13 : 18;
    for (int32 Item = 0; Item < Count; ++Item)
    {
        const float T = (Item + 0.35f) / Count;
        const float Side = (Item % 2 == 0) ? 1.0f : -1.0f;
        const float Offset = 900.0f + 150.0f * ((Item * 37 + Index * 19) % 7);
        const FVector Base = FMath::Lerp(Start, End, T) + Right * Side * Offset;
        if ((Item + Index) % 4 == 0)
        {
            AddRock(Base + FVector(0.0f, 0.0f, 38.0f), 0.75f + 0.08f * Item);
        }
        else
        {
            AddShrub(Base + FVector(0.0f, 0.0f, 58.0f), 0.55f + 0.06f * ((Item + Index) % 5), FLinearColor(0.04f, 0.18f + 0.02f * (Item % 3), 0.035f));
        }

        if (Index >= 3 && Item % 3 == 1)
        {
            AddTree(Base + Right * Side * 520.0f + FVector(0.0f, 0.0f, 95.0f), 1.0f + 0.08f * ((Item + Index) % 4));
        }

        if (Index >= 5 && Item % 4 == 2)
        {
            AddCactus(Base - Right * Side * 410.0f + FVector(0.0f, 0.0f, 65.0f), 0.65f + 0.04f * ((Item + 2) % 5));
        }
    }
}

void AJCRWorldBuilder::AddUtilityPole(const FVector& Location, float Yaw)
{
    AddMesh(TEXT("UtilityPole"), CylinderMesh, Location, FVector(0.16f, 0.16f, 5.2f), FRotator(0.0f, Yaw, 0.0f), HouseMaterial);
    AddBox(TEXT("UtilityCrossbar"), Location + FVector(0.0f, 0.0f, 265.0f), FVector(1.5f, 0.12f, 0.12f), FRotator(0.0f, Yaw, 0.0f), HouseMaterial);
    AddBox(TEXT("WireHint"), Location + FVector(190.0f, 0.0f, 278.0f), FVector(4.0f, 0.025f, 0.025f), FRotator(0.0f, Yaw, 0.0f), AccentMaterial);
}

void AJCRWorldBuilder::AddHouseCluster(const FVector& Location, float Yaw)
{
    AddBox(TEXT("CountryHouse_Base"), Location, FVector(3.6f, 2.4f, 1.6f), FRotator(0.0f, Yaw, 0.0f), HouseMaterial);
    AddBox(TEXT("CountryHouse_Roof"), Location + FVector(0.0f, 0.0f, 210.0f), FVector(4.0f, 2.8f, 0.35f), FRotator(0.0f, Yaw, 0.0f), RoofMaterial);
    AddBox(TEXT("CountryHouse_Annex"), Location + FVector(520.0f, 120.0f, 10.0f), FVector(2.6f, 1.8f, 1.1f), FRotator(0.0f, Yaw + 6.0f, 0.0f), HouseMaterial);
}

void AJCRWorldBuilder::AddToriiGate(const FVector& Location, float Yaw)
{
    AddMesh(TEXT("Torii_LeftPost"), CylinderMesh, Location + FVector(0.0f, -260.0f, 0.0f), FVector(0.28f, 0.28f, 5.0f), FRotator(0.0f, Yaw, 0.0f), AccentMaterial);
    AddMesh(TEXT("Torii_RightPost"), CylinderMesh, Location + FVector(0.0f, 260.0f, 0.0f), FVector(0.28f, 0.28f, 5.0f), FRotator(0.0f, Yaw, 0.0f), AccentMaterial);
    AddBox(TEXT("Torii_TopBeam"), Location + FVector(0.0f, 0.0f, 510.0f), FVector(0.42f, 7.2f, 0.32f), FRotator(0.0f, Yaw, 0.0f), AccentMaterial);
    AddBox(TEXT("Torii_MidBeam"), Location + FVector(0.0f, 0.0f, 400.0f), FVector(0.32f, 5.8f, 0.22f), FRotator(0.0f, Yaw, 0.0f), AccentMaterial);
}

void AJCRWorldBuilder::AddShrub(const FVector& Location, float Scale, const FLinearColor& Color)
{
    AddMesh(TEXT("Shrub_A"), SphereMesh, Location, FVector(Scale * 1.1f, Scale * 0.9f, Scale * 0.55f), FRotator::ZeroRotator, FoliageMaterial, false);
    AddMesh(TEXT("Shrub_B"), SphereMesh, Location + FVector(38.0f, 22.0f, 18.0f), FVector(Scale * 0.85f, Scale * 0.75f, Scale * 0.48f), FRotator::ZeroRotator, FoliageMaterial, false);
}

void AJCRWorldBuilder::AddTree(const FVector& Location, float Scale)
{
    AddMesh(TEXT("CedarTrunk"), CylinderMesh, Location + FVector(0.0f, 0.0f, 90.0f * Scale), FVector(0.16f * Scale, 0.16f * Scale, 2.1f * Scale), FRotator::ZeroRotator, HouseMaterial);
    AddMesh(TEXT("CedarCrown_Low"), ConeMesh, Location + FVector(0.0f, 0.0f, 260.0f * Scale), FVector(1.25f * Scale, 1.25f * Scale, 2.15f * Scale), FRotator::ZeroRotator, FoliageMaterial, false);
    AddMesh(TEXT("CedarCrown_High"), ConeMesh, Location + FVector(0.0f, 0.0f, 395.0f * Scale), FVector(0.95f * Scale, 0.95f * Scale, 1.65f * Scale), FRotator::ZeroRotator, FoliageMaterial, false);
}

void AJCRWorldBuilder::AddCactus(const FVector& Location, float Scale)
{
    AddMesh(TEXT("DesertCactus_Body"), CylinderMesh, Location + FVector(0.0f, 0.0f, 95.0f * Scale), FVector(0.18f * Scale, 0.18f * Scale, 2.1f * Scale), FRotator::ZeroRotator, FoliageMaterial, false);
    AddMesh(TEXT("DesertCactus_ArmL"), CylinderMesh, Location + FVector(0.0f, -42.0f * Scale, 150.0f * Scale), FVector(0.09f * Scale, 0.09f * Scale, 0.9f * Scale), FRotator(0.0f, 0.0f, 72.0f), FoliageMaterial, false);
    AddMesh(TEXT("DesertCactus_ArmR"), CylinderMesh, Location + FVector(0.0f, 42.0f * Scale, 185.0f * Scale), FVector(0.09f * Scale, 0.09f * Scale, 0.75f * Scale), FRotator(0.0f, 0.0f, -72.0f), FoliageMaterial, false);
}

void AJCRWorldBuilder::AddRock(const FVector& Location, float Scale)
{
    AddMesh(TEXT("RoadsideRock"), SphereMesh, Location, FVector(Scale * 1.4f, Scale, Scale * 0.45f), FRotator(0.0f, 25.0f, 0.0f), RockMaterial);
}

void AJCRWorldBuilder::AddMountainRidge()
{
    UMaterialInterface* FarMountain = MountainFarMaterial;
    for (int32 Index = 0; Index < 9; ++Index)
    {
        const float X = 62000.0f + Index * 16500.0f;
        const float Y = 38000.0f + FMath::Sin(Index * 0.7f) * 6200.0f;
        const float Height = 38.0f + (Index % 4) * 9.0f;
        AddMesh(TEXT("MountainCone"), ConeMesh, FVector(X, Y, 1100.0f + Height * 45.0f), FVector(62.0f + Index * 2.2f, 58.0f + Index * 1.8f, Height), FRotator(0.0f, -18.0f, 0.0f), FarMountain);
    }
}

void AJCRWorldBuilder::AddFinishGate(const FVector& Location, float Yaw)
{
    AddBox(TEXT("Finish_Left"), Location + FVector(0.0f, -420.0f, 0.0f), FVector(0.22f, 0.22f, 5.6f), FRotator(0.0f, Yaw, 0.0f), AccentMaterial);
    AddBox(TEXT("Finish_Right"), Location + FVector(0.0f, 420.0f, 0.0f), FVector(0.22f, 0.22f, 5.6f), FRotator(0.0f, Yaw, 0.0f), AccentMaterial);
    AddBox(TEXT("Finish_Banner"), Location + FVector(0.0f, 0.0f, 560.0f), FVector(0.3f, 8.8f, 0.32f), FRotator(0.0f, Yaw, 0.0f), AccentMaterial);
}

void AJCRWorldBuilder::SpawnRuntimeCheckpoints()
{
    const TArray<FVector> Points = CoursePoints();
    for (int32 Index = 0; Index < Points.Num(); ++Index)
    {
        const FVector Direction = Index < Points.Num() - 1 ? Points[Index + 1] - Points[Index] : Points[Index] - Points[Index - 1];
        const FRotator Rotation(0.0f, Direction.Rotation().Yaw, 0.0f);
        AJCRRaceCheckpoint* Checkpoint = GetWorld()->SpawnActor<AJCRRaceCheckpoint>(AJCRRaceCheckpoint::StaticClass(), Points[Index] + FVector(0.0f, 0.0f, 230.0f), Rotation);
        if (Checkpoint)
        {
            Checkpoint->CheckpointIndex = Index;
            Checkpoint->bFinish = Index == Points.Num() - 1;
        }
    }
}
