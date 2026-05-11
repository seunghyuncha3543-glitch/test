#include "JCRVehiclePawn.h"

#include "Camera/CameraComponent.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "CollisionQueryParams.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"
#include "JCREngineSynth.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NiagaraComponent.h"
#include "UObject/ConstructorHelpers.h"

AJCRVehiclePawn::AJCRVehiclePawn()
{
    PrimaryActorTick.bCanEverTick = true;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereFinder(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderFinder(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SportsCarBodyFinder(TEXT("/Game/SportsCar/SM_SportsCar.SM_SportsCar"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SportsCarGlassFinder(TEXT("/Game/SportsCar/SM_SportsCar_Glass.SM_SportsCar_Glass"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SportsCarWheelFinder(TEXT("/Game/SportsCar/SM_SportsCar_Wheel.SM_SportsCar_Wheel"));
    UStaticMesh* Cube = CubeFinder.Object;
    UStaticMesh* Sphere = SphereFinder.Object;
    UStaticMesh* Cylinder = CylinderFinder.Object;
    UStaticMesh* SportsCarBody = SportsCarBodyFinder.Object;
    UStaticMesh* SportsCarGlass = SportsCarGlassFinder.Object;
    UStaticMesh* SportsCarWheel = SportsCarWheelFinder.Object;
    bTemplateSportsCarVisualAvailable = SportsCarBody != nullptr && SportsCarWheel != nullptr;

    PrototypeBody = CreatePrototypePart(TEXT("PrototypeBody"), Cube, FVector(0.0f, 0.0f, 55.0f), FVector(4.65f, 1.9f, 0.46f), FRotator::ZeroRotator);
    PrototypeNose = CreatePrototypePart(TEXT("PrototypeNose"), Cube, FVector(245.0f, 0.0f, 38.0f), FVector(1.25f, 1.68f, 0.28f), FRotator(0.0f, 0.0f, -4.0f));
    PrototypeCabin = CreatePrototypePart(TEXT("PrototypeCabin"), Sphere, FVector(-50.0f, 0.0f, 110.0f), FVector(1.65f, 1.05f, 0.42f), FRotator::ZeroRotator);
    PrototypeRearDeck = CreatePrototypePart(TEXT("PrototypeRearDeck"), Cube, FVector(-245.0f, 0.0f, 72.0f), FVector(1.25f, 1.82f, 0.35f), FRotator(0.0f, 0.0f, 3.0f));
    PrototypeWing = CreatePrototypePart(TEXT("PrototypeWing"), Cube, FVector(-330.0f, 0.0f, 145.0f), FVector(0.18f, 2.45f, 0.08f), FRotator::ZeroRotator);
    PrototypeSplitter = CreatePrototypePart(TEXT("PrototypeSplitter"), Cube, FVector(325.0f, 0.0f, 18.0f), FVector(0.12f, 2.25f, 0.07f), FRotator::ZeroRotator);
    PrototypeWheelFL = CreatePrototypePart(TEXT("PrototypeWheelFL"), Cylinder, FVector(155.0f, -112.0f, 27.0f), FVector(0.46f, 0.46f, 0.32f), FRotator(90.0f, 0.0f, 0.0f));
    PrototypeWheelFR = CreatePrototypePart(TEXT("PrototypeWheelFR"), Cylinder, FVector(155.0f, 112.0f, 27.0f), FVector(0.46f, 0.46f, 0.32f), FRotator(90.0f, 0.0f, 0.0f));
    PrototypeWheelRL = CreatePrototypePart(TEXT("PrototypeWheelRL"), Cylinder, FVector(-185.0f, -116.0f, 29.0f), FVector(0.52f, 0.52f, 0.36f), FRotator(90.0f, 0.0f, 0.0f));
    PrototypeWheelRR = CreatePrototypePart(TEXT("PrototypeWheelRR"), Cylinder, FVector(-185.0f, 116.0f, 29.0f), FVector(0.52f, 0.52f, 0.36f), FRotator(90.0f, 0.0f, 0.0f));

    ApplyPrototypeMaterial(PrototypeBody, TEXT("BodyGraphite"), FLinearColor(0.12f, 0.13f, 0.135f), 0.19f, 0.55f);
    ApplyPrototypeMaterial(PrototypeNose, TEXT("BodyGraphiteNose"), FLinearColor(0.16f, 0.165f, 0.17f), 0.17f, 0.58f);
    ApplyPrototypeMaterial(PrototypeRearDeck, TEXT("BodyGraphiteRear"), FLinearColor(0.10f, 0.105f, 0.11f), 0.22f, 0.52f);
    ApplyPrototypeMaterial(PrototypeCabin, TEXT("GlassBlack"), FLinearColor(0.01f, 0.018f, 0.03f), 0.04f, 0.0f);
    ApplyPrototypeMaterial(PrototypeWing, TEXT("CarbonWing"), FLinearColor(0.005f, 0.005f, 0.006f), 0.42f, 0.1f);
    ApplyPrototypeMaterial(PrototypeSplitter, TEXT("CarbonSplitter"), FLinearColor(0.005f, 0.005f, 0.006f), 0.38f, 0.1f);
    ApplyPrototypeMaterial(PrototypeWheelFL, TEXT("RubberFL"), FLinearColor(0.004f, 0.004f, 0.004f), 0.72f, 0.0f);
    ApplyPrototypeMaterial(PrototypeWheelFR, TEXT("RubberFR"), FLinearColor(0.004f, 0.004f, 0.004f), 0.72f, 0.0f);
    ApplyPrototypeMaterial(PrototypeWheelRL, TEXT("RubberRL"), FLinearColor(0.004f, 0.004f, 0.004f), 0.72f, 0.0f);
    ApplyPrototypeMaterial(PrototypeWheelRR, TEXT("RubberRR"), FLinearColor(0.004f, 0.004f, 0.004f), 0.72f, 0.0f);

    TemplateSportsCarBody = CreatePrototypePart(TEXT("TemplateSportsCarBody"), SportsCarBody, FVector(0.0f, 0.0f, 22.0f), FVector(1.0f, 1.0f, 1.0f), FRotator::ZeroRotator);
    TemplateSportsCarGlass = CreatePrototypePart(TEXT("TemplateSportsCarGlass"), SportsCarGlass, FVector(0.0f, 0.0f, 22.0f), FVector(1.0f, 1.0f, 1.0f), FRotator::ZeroRotator);
    TemplateWheelFL = CreatePrototypePart(TEXT("TemplateWheelFL"), SportsCarWheel, FVector(142.0f, -92.0f, 38.0f), FVector(1.0f, 1.0f, 1.0f), FRotator(0.0f, 0.0f, 0.0f));
    TemplateWheelFR = CreatePrototypePart(TEXT("TemplateWheelFR"), SportsCarWheel, FVector(142.0f, 92.0f, 38.0f), FVector(1.0f, -1.0f, 1.0f), FRotator(0.0f, 0.0f, 0.0f));
    TemplateWheelRL = CreatePrototypePart(TEXT("TemplateWheelRL"), SportsCarWheel, FVector(-155.0f, -92.0f, 38.0f), FVector(1.0f, 1.0f, 1.0f), FRotator(0.0f, 0.0f, 0.0f));
    TemplateWheelRR = CreatePrototypePart(TEXT("TemplateWheelRR"), SportsCarWheel, FVector(-155.0f, 92.0f, 38.0f), FVector(1.0f, -1.0f, 1.0f), FRotator(0.0f, 0.0f, 0.0f));

    ChaseArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("ChaseArm"));
    ChaseArm->SetupAttachment(GetMesh());
    ChaseArm->TargetArmLength = 590.0f;
    ChaseArm->SocketOffset = FVector(0.0f, 0.0f, 95.0f);
    ChaseArm->SetRelativeRotation(FRotator(-8.0f, 0.0f, 0.0f));
    ChaseArm->bEnableCameraLag = true;
    ChaseArm->CameraLagSpeed = 10.0f;
    ChaseArm->bEnableCameraRotationLag = true;
    ChaseArm->CameraRotationLagSpeed = 9.0f;

    ChaseCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ChaseCamera"));
    ChaseCamera->SetupAttachment(ChaseArm, USpringArmComponent::SocketName);
    ChaseCamera->FieldOfView = BaseFov;

    LeftHeadlight = CreateDefaultSubobject<UPointLightComponent>(TEXT("LeftHeadlight"));
    LeftHeadlight->SetupAttachment(GetMesh());
    LeftHeadlight->SetRelativeLocation(FVector(210.0f, -62.0f, 38.0f));
    LeftHeadlight->Intensity = 2200.0f;
    LeftHeadlight->AttenuationRadius = 950.0f;
    LeftHeadlight->SourceRadius = 22.0f;

    RightHeadlight = CreateDefaultSubobject<UPointLightComponent>(TEXT("RightHeadlight"));
    RightHeadlight->SetupAttachment(GetMesh());
    RightHeadlight->SetRelativeLocation(FVector(210.0f, 62.0f, 38.0f));
    RightHeadlight->Intensity = 2200.0f;
    RightHeadlight->AttenuationRadius = 950.0f;
    RightHeadlight->SourceRadius = 22.0f;

    LeftBrakeLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("LeftBrakeLight"));
    LeftBrakeLight->SetupAttachment(GetMesh());
    LeftBrakeLight->SetRelativeLocation(FVector(-220.0f, -64.0f, 45.0f));
    LeftBrakeLight->SetLightColor(FLinearColor(1.0f, 0.02f, 0.0f));
    LeftBrakeLight->Intensity = 0.0f;
    LeftBrakeLight->AttenuationRadius = 250.0f;

    RightBrakeLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("RightBrakeLight"));
    RightBrakeLight->SetupAttachment(GetMesh());
    RightBrakeLight->SetRelativeLocation(FVector(-220.0f, 64.0f, 45.0f));
    RightBrakeLight->SetLightColor(FLinearColor(1.0f, 0.02f, 0.0f));
    RightBrakeLight->Intensity = 0.0f;
    RightBrakeLight->AttenuationRadius = 250.0f;

    DustTrail = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DustTrail"));
    DustTrail->SetupAttachment(GetMesh());
    DustTrail->SetRelativeLocation(FVector(-170.0f, 0.0f, -25.0f));
    DustTrail->bAutoActivate = false;

    ExhaustEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ExhaustEffect"));
    ExhaustEffect->SetupAttachment(GetMesh());
    ExhaustEffect->SetRelativeLocation(FVector(-225.0f, 48.0f, 20.0f));
    ExhaustEffect->bAutoActivate = true;

    EngineSynth = CreateDefaultSubobject<UJCREngineSynth>(TEXT("EngineSynth"));
    EngineSynth->SetupAttachment(GetRootComponent());

    ConfigureRuntimeInput();
    ConfigureVehicle();
}

void AJCRVehiclePawn::BeginPlay()
{
    Super::BeginPlay();
    TeleportToStart();
    SetPrototypeVisible(IsUsingPrototypeFallback() && !bTemplateSportsCarVisualAvailable);
    SetTemplateSportsCarVisible(IsUsingPrototypeFallback() && bTemplateSportsCarVisualAvailable);
    if (EngineSynth)
    {
        EngineSynth->Start();
    }

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                Subsystem->AddMappingContext(DrivingMappingContext, 0);
            }
        }
    }
}

void AJCRVehiclePawn::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    UpdatePrototypeFallback(DeltaSeconds);
    UpdateCamera(DeltaSeconds);
    UpdateLightsAndEffects();
    if (EngineSynth)
    {
        EngineSynth->SetDrivingState(GetSpeedKmh(), ThrottleInput, BrakeInput, bGrounded);
    }
}

void AJCRVehiclePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
    Input->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &AJCRVehiclePawn::ApplyThrottle);
    Input->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &AJCRVehiclePawn::ApplyThrottle);
    Input->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &AJCRVehiclePawn::ApplyBrake);
    Input->BindAction(BrakeAction, ETriggerEvent::Completed, this, &AJCRVehiclePawn::ApplyBrake);
    Input->BindAction(SteerLeftAction, ETriggerEvent::Triggered, this, &AJCRVehiclePawn::ApplySteerLeft);
    Input->BindAction(SteerLeftAction, ETriggerEvent::Completed, this, &AJCRVehiclePawn::StopSteerLeft);
    Input->BindAction(SteerRightAction, ETriggerEvent::Triggered, this, &AJCRVehiclePawn::ApplySteerRight);
    Input->BindAction(SteerRightAction, ETriggerEvent::Completed, this, &AJCRVehiclePawn::StopSteerRight);
    Input->BindAction(SteerAxisAction, ETriggerEvent::Triggered, this, &AJCRVehiclePawn::ApplySteerAxis);
    Input->BindAction(SteerAxisAction, ETriggerEvent::Completed, this, &AJCRVehiclePawn::ApplySteerAxis);
    Input->BindAction(ToggleAbsAction, ETriggerEvent::Started, this, &AJCRVehiclePawn::ToggleAbs);
    Input->BindAction(ToggleTcsAction, ETriggerEvent::Started, this, &AJCRVehiclePawn::ToggleTcs);
    Input->BindAction(ResetAction, ETriggerEvent::Started, this, &AJCRVehiclePawn::ResetVehicle);
    Input->BindAction(HandbrakeAction, ETriggerEvent::Triggered, this, &AJCRVehiclePawn::ApplyHandbrake);
    Input->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &AJCRVehiclePawn::ApplyHandbrake);
    Input->BindAction(SportModeAction, ETriggerEvent::Started, this, &AJCRVehiclePawn::ToggleSportMode);
    Input->BindAction(CameraModeAction, ETriggerEvent::Started, this, &AJCRVehiclePawn::CycleCameraMode);
}

float AJCRVehiclePawn::GetSpeedKmh() const
{
    if (IsUsingPrototypeFallback())
    {
        return FMath::Abs(PrototypeSpeedCms) * 0.036f;
    }
    return FMath::Abs(GetVehicleMovementComponent()->GetForwardSpeed()) * 0.036f;
}

float AJCRVehiclePawn::GetDisplayRpm() const
{
    return SimulatedRpm;
}

int32 AJCRVehiclePawn::GetDisplayGear() const
{
    return SimulatedGear;
}

void AJCRVehiclePawn::ConfigureRuntimeInput()
{
    DrivingMappingContext = CreateDefaultSubobject<UInputMappingContext>(TEXT("IMC_Driving_Runtime"));
    ThrottleAction = CreateDefaultSubobject<UInputAction>(TEXT("IA_Throttle"));
    BrakeAction = CreateDefaultSubobject<UInputAction>(TEXT("IA_Brake"));
    SteerLeftAction = CreateDefaultSubobject<UInputAction>(TEXT("IA_SteerLeft"));
    SteerRightAction = CreateDefaultSubobject<UInputAction>(TEXT("IA_SteerRight"));
    SteerAxisAction = CreateDefaultSubobject<UInputAction>(TEXT("IA_SteerAxis"));
    ToggleAbsAction = CreateDefaultSubobject<UInputAction>(TEXT("IA_ToggleABS"));
    ToggleTcsAction = CreateDefaultSubobject<UInputAction>(TEXT("IA_ToggleTCS"));
    ResetAction = CreateDefaultSubobject<UInputAction>(TEXT("IA_ResetVehicle"));
    HandbrakeAction = CreateDefaultSubobject<UInputAction>(TEXT("IA_Handbrake"));
    SportModeAction = CreateDefaultSubobject<UInputAction>(TEXT("IA_SportMode"));
    CameraModeAction = CreateDefaultSubobject<UInputAction>(TEXT("IA_CameraMode"));

    ThrottleAction->ValueType = EInputActionValueType::Axis1D;
    BrakeAction->ValueType = EInputActionValueType::Axis1D;
    SteerLeftAction->ValueType = EInputActionValueType::Boolean;
    SteerRightAction->ValueType = EInputActionValueType::Boolean;
    SteerAxisAction->ValueType = EInputActionValueType::Axis1D;
    ToggleAbsAction->ValueType = EInputActionValueType::Boolean;
    ToggleTcsAction->ValueType = EInputActionValueType::Boolean;
    ResetAction->ValueType = EInputActionValueType::Boolean;
    HandbrakeAction->ValueType = EInputActionValueType::Boolean;
    SportModeAction->ValueType = EInputActionValueType::Boolean;
    CameraModeAction->ValueType = EInputActionValueType::Boolean;

    DrivingMappingContext->MapKey(ThrottleAction, EKeys::W);
    DrivingMappingContext->MapKey(ThrottleAction, EKeys::Gamepad_RightTriggerAxis);
    DrivingMappingContext->MapKey(BrakeAction, EKeys::S);
    DrivingMappingContext->MapKey(BrakeAction, EKeys::Gamepad_LeftTriggerAxis);
    DrivingMappingContext->MapKey(SteerLeftAction, EKeys::A);
    DrivingMappingContext->MapKey(SteerRightAction, EKeys::D);
    DrivingMappingContext->MapKey(SteerAxisAction, EKeys::Gamepad_LeftX);
    DrivingMappingContext->MapKey(ToggleAbsAction, EKeys::One);
    DrivingMappingContext->MapKey(ToggleTcsAction, EKeys::Two);
    DrivingMappingContext->MapKey(ResetAction, EKeys::R);
    DrivingMappingContext->MapKey(HandbrakeAction, EKeys::SpaceBar);
    DrivingMappingContext->MapKey(HandbrakeAction, EKeys::Gamepad_FaceButton_Right);
    DrivingMappingContext->MapKey(SportModeAction, EKeys::LeftShift);
    DrivingMappingContext->MapKey(CameraModeAction, EKeys::C);
}

void AJCRVehiclePawn::ConfigureVehicle()
{
    if (UChaosWheeledVehicleMovementComponent* Move = Cast<UChaosWheeledVehicleMovementComponent>(GetVehicleMovementComponent()))
    {
        Move->Mass = VehicleMassKg;
        Move->EngineSetup.MaxTorque = MaxTorqueNm;
        Move->TransmissionSetup.bUseAutomaticGears = true;
        Move->TransmissionSetup.FinalRatio = 3.08f;
        Move->TransmissionSetup.ForwardGearRatios = { 3.91f, 2.29f, 1.58f, 1.18f, 0.94f, 0.79f, 0.67f, 0.56f };
        Move->TransmissionSetup.ReverseGearRatios = { 3.3f };
        Move->DifferentialSetup.DifferentialType = EVehicleDifferential::AllWheelDrive;
        Move->DifferentialSetup.FrontRearSplit = 0.36f;
    }
}

void AJCRVehiclePawn::ApplyThrottle(const FInputActionValue& Value)
{
    ThrottleInput = FMath::Clamp(Value.Get<float>(), 0.0f, 1.0f);
    GetVehicleMovementComponent()->SetThrottleInput(ThrottleInput);
}

void AJCRVehiclePawn::ApplyBrake(const FInputActionValue& Value)
{
    BrakeInput = FMath::Clamp(Value.Get<float>(), 0.0f, 1.0f);
    GetVehicleMovementComponent()->SetBrakeInput(BrakeInput);
}

void AJCRVehiclePawn::ApplySteerLeft(const FInputActionValue& Value)
{
    bLeftHeld = Value.Get<bool>();
    RecomputeSteering();
}

void AJCRVehiclePawn::ApplySteerRight(const FInputActionValue& Value)
{
    bRightHeld = Value.Get<bool>();
    RecomputeSteering();
}

void AJCRVehiclePawn::ApplySteerAxis(const FInputActionValue& Value)
{
    GamepadSteer = FMath::Clamp(Value.Get<float>(), -1.0f, 1.0f);
    RecomputeSteering();
}

void AJCRVehiclePawn::StopSteerLeft(const FInputActionValue& Value)
{
    bLeftHeld = false;
    RecomputeSteering();
}

void AJCRVehiclePawn::StopSteerRight(const FInputActionValue& Value)
{
    bRightHeld = false;
    RecomputeSteering();
}

void AJCRVehiclePawn::ToggleAbs(const FInputActionValue& Value)
{
    bAbsEnabled = !bAbsEnabled;
}

void AJCRVehiclePawn::ToggleTcs(const FInputActionValue& Value)
{
    bTcsEnabled = !bTcsEnabled;
}

void AJCRVehiclePawn::ResetVehicle(const FInputActionValue& Value)
{
    TeleportToStart();
}

void AJCRVehiclePawn::ApplyHandbrake(const FInputActionValue& Value)
{
    bHandbrakeHeld = Value.Get<bool>();
}

void AJCRVehiclePawn::ToggleSportMode(const FInputActionValue& Value)
{
    bSportModeEnabled = !bSportModeEnabled;
}

void AJCRVehiclePawn::CycleCameraMode(const FInputActionValue& Value)
{
    CameraMode = (CameraMode + 1) % 4;
}

void AJCRVehiclePawn::RecomputeSteering()
{
    KeyboardSteer = 0.0f;
    if (bLeftHeld)
    {
        KeyboardSteer -= 1.0f;
    }
    if (bRightHeld)
    {
        KeyboardSteer += 1.0f;
    }

    const float RawSteer = FMath::Abs(GamepadSteer) > 0.05f ? GamepadSteer : KeyboardSteer;
    const float SpeedAlpha = FMath::Clamp(GetSpeedKmh() / 180.0f, 0.0f, 1.0f);
    const float SteeringScale = FMath::Lerp(1.0f, HighSpeedSteeringScale, SpeedAlpha);
    GetVehicleMovementComponent()->SetSteeringInput(RawSteer * SteeringScale);
}

void AJCRVehiclePawn::UpdateCamera(float DeltaSeconds)
{
    const float SpeedAlpha = FMath::Clamp(GetSpeedKmh() / 240.0f, 0.0f, 1.0f);
    const float AccelBias = FMath::Clamp(ThrottleInput - BrakeInput * 0.6f, -1.0f, 1.0f);
    FVector TargetSocket(0.0f, 0.0f, 95.0f);
    float TargetArm = FMath::Lerp(520.0f, 665.0f, SpeedAlpha) + AccelBias * 24.0f;
    float TargetPitch = FMath::Lerp(-7.5f, -4.0f, SpeedAlpha);
    float TargetFov = FMath::Lerp(BaseFov, MaxFov, SpeedAlpha);

    if (CameraMode == 1)
    {
        TargetArm = 360.0f;
        TargetSocket = FVector(0.0f, 0.0f, 82.0f);
        TargetFov += 3.0f;
    }
    else if (CameraMode == 2)
    {
        TargetArm = 32.0f;
        TargetSocket = FVector(260.0f, 0.0f, 48.0f);
        TargetPitch = -1.0f;
        TargetFov = FMath::Lerp(82.0f, 100.0f, SpeedAlpha);
    }
    else if (CameraMode == 3)
    {
        TargetArm = 74.0f;
        TargetSocket = FVector(110.0f, 0.0f, 118.0f);
        TargetPitch = -3.0f;
        TargetFov = FMath::Lerp(76.0f, 94.0f, SpeedAlpha);
    }

    ChaseCamera->SetFieldOfView(FMath::FInterpTo(ChaseCamera->FieldOfView, TargetFov, DeltaSeconds, 4.0f));
    ChaseArm->TargetArmLength = FMath::FInterpTo(ChaseArm->TargetArmLength, TargetArm, DeltaSeconds, 6.0f);
    ChaseArm->SocketOffset = FMath::VInterpTo(ChaseArm->SocketOffset, TargetSocket, DeltaSeconds, 7.0f);
    ChaseArm->SetRelativeRotation(FMath::RInterpTo(ChaseArm->GetRelativeRotation(), FRotator(TargetPitch, 0.0f, 0.0f), DeltaSeconds, 7.0f));
}

void AJCRVehiclePawn::UpdateLightsAndEffects()
{
    const float BrakeIntensity = BrakeInput > 0.05f ? 4200.0f : 320.0f;
    LeftBrakeLight->SetIntensity(BrakeIntensity);
    RightBrakeLight->SetIntensity(BrakeIntensity);

    const bool bDust = GetSpeedKmh() > 16.0f && FMath::Abs(GetVehicleMovementComponent()->GetForwardSpeed()) > 1.0f;
    const bool bPrototypeDust = IsUsingPrototypeFallback() && GetSpeedKmh() > 16.0f;
    DustTrail->SetActive(bDust || bPrototypeDust);
    ExhaustEffect->SetFloatParameter(TEXT("Throttle"), ThrottleInput);
}

bool AJCRVehiclePawn::IsUsingPrototypeFallback() const
{
    return true;
}

void AJCRVehiclePawn::UpdatePrototypeFallback(float DeltaSeconds)
{
    if (!IsUsingPrototypeFallback())
    {
        return;
    }

    KeyboardSteer = 0.0f;
    if (bLeftHeld)
    {
        KeyboardSteer -= 1.0f;
    }
    if (bRightHeld)
    {
        KeyboardSteer += 1.0f;
    }

    const float RawSteer = FMath::Abs(GamepadSteer) > 0.05f ? GamepadSteer : KeyboardSteer;
    const float SpeedKmh = GetSpeedKmh();
    const float SpeedMps = FMath::Abs(PrototypeSpeedCms) * 0.01f;
    const float SpeedAlpha = FMath::Clamp(SpeedKmh / 280.0f, 0.0f, 1.0f);
    const float SteeringScale = FMath::Lerp(1.0f, HighSpeedSteeringScale, SpeedAlpha);

    const float GearUpshiftRpm = bSportModeEnabled ? 7600.0f : 6500.0f;
    const float GearDownshiftRpm = bSportModeEnabled ? 2700.0f : 2200.0f;
    const float GearRatios[8] = { 3.91f, 2.29f, 1.58f, 1.18f, 0.94f, 0.79f, 0.67f, 0.56f };
    const float FinalDrive = 3.08f;
    const float WheelRadiusM = 0.34f;
    const float WheelRpm = (SpeedMps / (2.0f * PI * WheelRadiusM)) * 60.0f;
    const float CurrentRatio = GearRatios[FMath::Clamp(SimulatedGear - 1, 0, 7)] * FinalDrive;
    const float TargetRpm = FMath::Clamp(1050.0f + WheelRpm * CurrentRatio, 950.0f, 8600.0f);
    SimulatedRpm = FMath::FInterpTo(SimulatedRpm, FMath::Max(TargetRpm, 1100.0f + ThrottleInput * 1700.0f), DeltaSeconds, 9.0f);

    ShiftTimer = FMath::Max(0.0f, ShiftTimer - DeltaSeconds);
    if (ShiftTimer <= 0.0f && SimulatedGear < 8 && SimulatedRpm > GearUpshiftRpm)
    {
        ++SimulatedGear;
        ShiftTimer = 0.16f;
        SimulatedRpm *= 0.68f;
    }
    else if (ShiftTimer <= 0.0f && SimulatedGear > 1 && SimulatedRpm < GearDownshiftRpm && SpeedKmh > 12.0f)
    {
        --SimulatedGear;
        ShiftTimer = 0.12f;
        SimulatedRpm *= 1.28f;
    }

    const float RpmAlpha = FMath::Clamp((SimulatedRpm - 1000.0f) / 7000.0f, 0.0f, 1.0f);
    const float TorqueCurve = FMath::Lerp(0.62f, 1.0f, FMath::Sin(RpmAlpha * PI));
    const float ThrottleResponse = bSportModeEnabled ? FMath::Pow(ThrottleInput, 0.72f) : FMath::Pow(ThrottleInput, 1.18f);
    const float ShiftCut = ShiftTimer > 0.0f ? 0.38f : 1.0f;
    const float TractionLimit = bGrounded ? (bTcsEnabled ? 1.0f : 1.18f) : 0.18f;
    const float EngineForce = ThrottleResponse * MaxTorqueNm * TorqueCurve * CurrentRatio * 42.0f * ShiftCut * TractionLimit;
    const float DragForce = 0.30f * SpeedMps * SpeedMps * 58.0f;
    const float RollingResistance = (bGrounded ? 13.5f : 3.0f) * SpeedMps;

    BrakeTemperature = FMath::Clamp(BrakeTemperature + BrakeInput * SpeedAlpha * 65.0f * DeltaSeconds - 18.0f * DeltaSeconds, 0.0f, 1.0f);
    const float Fade = FMath::Lerp(1.0f, 0.72f, BrakeTemperature);
    const float AbsPulse = (!bAbsEnabled && BrakeInput > 0.82f && SpeedKmh > 55.0f) ? 1.25f : 1.0f;
    const float BrakeForce = BrakeInput * 11800.0f * Fade * (bHandbrakeHeld ? 1.45f : 1.0f) * AbsPulse;
    const float NetForce = EngineForce - DragForce - RollingResistance - BrakeForce;
    const float AccelCms = (NetForce / VehicleMassKg) * 100.0f;
    PrototypeSpeedCms = FMath::Clamp(PrototypeSpeedCms + AccelCms * DeltaSeconds, -1050.0f, 8700.0f);

    const float GripLoss = bHandbrakeHeld ? FMath::Lerp(0.45f, 0.22f, SpeedAlpha) : 1.0f;
    const float TurnRateDeg = FMath::Lerp(62.0f, 16.0f, SpeedAlpha) * RawSteer * SteeringScale * GripLoss;
    const float DriftImpulse = bHandbrakeHeld ? RawSteer * SpeedAlpha * 860.0f : RawSteer * SpeedAlpha * 125.0f;
    LateralSlipCms = FMath::FInterpTo(LateralSlipCms, DriftImpulse, DeltaSeconds, bHandbrakeHeld ? 2.4f : 5.5f);
    AddActorWorldRotation(FRotator(0.0f, TurnRateDeg * DeltaSeconds, 0.0f));
    AddActorWorldOffset((GetActorForwardVector() * PrototypeSpeedCms + GetActorRightVector() * LateralSlipCms) * DeltaSeconds, true);

    BodyRollDegrees = FMath::FInterpTo(BodyRollDegrees, -RawSteer * SpeedAlpha * 5.5f, DeltaSeconds, 5.0f);
    BodyPitchDegrees = FMath::FInterpTo(BodyPitchDegrees, BrakeInput * 3.2f - ThrottleInput * 1.8f, DeltaSeconds, 4.5f);

    FHitResult GroundHit;
    bGrounded = TraceGround(GroundHit, GetActorLocation());
    FVector CurrentLocation = GetActorLocation();
    if (bGrounded)
    {
        TimeWithoutGround = 0.0f;
        const float TargetZ = GroundHit.Location.Z + GroundClearanceCm;
        CurrentLocation.Z = FMath::FInterpTo(CurrentLocation.Z, TargetZ, DeltaSeconds, 18.0f);
        VerticalSpeedCms = 0.0f;
        SetActorLocation(CurrentLocation, false);

        const FRotator GroundRotation = MakeGroundAlignedRotation(GetActorRotation().Yaw, GroundHit.Normal);
        SetActorRotation(FMath::RInterpTo(GetActorRotation(), GroundRotation, DeltaSeconds, 8.5f));
    }
    else
    {
        TimeWithoutGround += DeltaSeconds;
        VerticalSpeedCms -= 980.0f * DeltaSeconds;
        CurrentLocation.Z += VerticalSpeedCms * DeltaSeconds;
        SetActorLocation(CurrentLocation, false);
        if (TimeWithoutGround > 0.45f || CurrentLocation.Z < -250.0f)
        {
            TeleportToStart();
        }
    }

    StabilizeOnCourse(DeltaSeconds);
    UpdateWheelVisuals(RawSteer, DeltaSeconds);
}

bool AJCRVehiclePawn::TraceGround(FHitResult& OutHit, const FVector& FromLocation) const
{
    if (!GetWorld())
    {
        return false;
    }

    const FVector Start = FromLocation + FVector(0.0f, 0.0f, 260.0f);
    const FVector End = FromLocation - FVector(0.0f, 0.0f, 420.0f);
    FCollisionQueryParams Params(SCENE_QUERY_STAT(JCRGroundTrace), false, this);
    return GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_WorldStatic, Params);
}

bool AJCRVehiclePawn::FindNearestGround(FHitResult& OutHit) const
{
    const FVector Origin = GetActorLocation();
    const TArray<FVector> ProbeOffsets = {
        FVector::ZeroVector,
        FVector(600.0f, 0.0f, 0.0f),
        FVector(1200.0f, 0.0f, 0.0f),
        FVector(0.0f, 450.0f, 0.0f),
        FVector(0.0f, -450.0f, 0.0f),
        FVector(1800.0f, 0.0f, 0.0f)
    };

    for (const FVector& Offset : ProbeOffsets)
    {
        if (TraceGround(OutHit, Origin + Offset))
        {
            return true;
        }
    }

    return false;
}

FVector AJCRVehiclePawn::FindNearestCoursePoint(const FVector& Location, float& OutDistanceToCourse) const
{
    const TArray<FVector> Points = {
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

    FVector BestPoint = Points[0];
    OutDistanceToCourse = TNumericLimits<float>::Max();
    const FVector FlatLocation(Location.X, Location.Y, 0.0f);
    for (int32 Index = 0; Index < Points.Num() - 1; ++Index)
    {
        const FVector A(Points[Index].X, Points[Index].Y, 0.0f);
        const FVector B(Points[Index + 1].X, Points[Index + 1].Y, 0.0f);
        const FVector Segment = B - A;
        const float Denom = FMath::Max(Segment.SizeSquared(), 1.0f);
        const float T = FMath::Clamp(FVector::DotProduct(FlatLocation - A, Segment) / Denom, 0.0f, 1.0f);
        const FVector Candidate = FMath::Lerp(Points[Index], Points[Index + 1], T);
        const float Distance = FVector::Dist2D(Location, Candidate);
        if (Distance < OutDistanceToCourse)
        {
            OutDistanceToCourse = Distance;
            BestPoint = Candidate;
        }
    }
    return BestPoint;
}

void AJCRVehiclePawn::StabilizeOnCourse(float DeltaSeconds)
{
    float DistanceToCourse = 0.0f;
    const FVector CoursePoint = FindNearestCoursePoint(GetActorLocation(), DistanceToCourse);
    if (DistanceToCourse > 980.0f)
    {
        const float PullAlpha = DistanceToCourse > 1900.0f ? 0.35f : 0.045f;
        FVector Current = GetActorLocation();
        Current.X = FMath::Lerp(Current.X, CoursePoint.X, PullAlpha);
        Current.Y = FMath::Lerp(Current.Y, CoursePoint.Y, PullAlpha);
        LateralSlipCms *= 0.55f;
        SetActorLocation(Current, false);
    }

    FHitResult GroundHit;
    if (TraceGround(GroundHit, GetActorLocation() + FVector(0.0f, 0.0f, 180.0f)))
    {
        FVector Current = GetActorLocation();
        const float TargetZ = GroundHit.Location.Z + GroundClearanceCm;
        if (FMath::Abs(Current.Z - TargetZ) > 85.0f)
        {
            Current.Z = FMath::FInterpTo(Current.Z, TargetZ, DeltaSeconds, 24.0f);
            SetActorLocation(Current, false);
            bGrounded = true;
            VerticalSpeedCms = 0.0f;
        }
    }
    else if (DistanceToCourse > 1500.0f || GetActorLocation().Z > CoursePoint.Z + 900.0f)
    {
        FVector Rescue = CoursePoint + FVector(0.0f, 0.0f, 260.0f);
        SetActorLocation(Rescue, false);
        FHitResult RescueHit;
        if (FindNearestGround(RescueHit))
        {
            SetActorLocation(RescueHit.Location + FVector(0.0f, 0.0f, GroundClearanceCm), false);
        }
        PrototypeSpeedCms *= 0.35f;
        LateralSlipCms = 0.0f;
        VerticalSpeedCms = 0.0f;
    }
}

FRotator AJCRVehiclePawn::MakeGroundAlignedRotation(float YawDegrees, const FVector& GroundNormal) const
{
    const FVector Forward = FRotationMatrix(FRotator(0.0f, YawDegrees, 0.0f)).GetScaledAxis(EAxis::X);
    const FVector ProjectedForward = FVector::VectorPlaneProject(Forward, GroundNormal).GetSafeNormal();
    return FRotationMatrix::MakeFromXZ(ProjectedForward, GroundNormal).Rotator();
}

void AJCRVehiclePawn::UpdateWheelVisuals(float RawSteer, float DeltaSeconds)
{
    const float WheelRadiusCm = 34.0f;
    WheelSpinDegrees = FMath::Fmod(WheelSpinDegrees + (PrototypeSpeedCms / WheelRadiusCm) * DeltaSeconds * (180.0f / PI), 360.0f);
    const float SteerDegrees = RawSteer * FMath::Lerp(28.0f, 9.0f, FMath::Clamp(GetSpeedKmh() / 180.0f, 0.0f, 1.0f));
    const float BrakeSquat = BrakeInput * FMath::Clamp(GetSpeedKmh() / 120.0f, 0.0f, 1.0f) * 4.0f;

    if (bTemplateSportsCarVisualAvailable)
    {
        TemplateSportsCarBody->SetRelativeLocation(FVector(0.0f, 0.0f, 22.0f - BrakeSquat));
        TemplateSportsCarGlass->SetRelativeLocation(FVector(0.0f, 0.0f, 22.0f - BrakeSquat));
        TemplateSportsCarBody->SetRelativeRotation(FRotator(BodyPitchDegrees, 0.0f, BodyRollDegrees));
        TemplateSportsCarGlass->SetRelativeRotation(FRotator(BodyPitchDegrees, 0.0f, BodyRollDegrees));

        TemplateWheelFL->SetRelativeLocation(FVector(142.0f, -92.0f, 38.0f - BrakeSquat * 0.25f));
        TemplateWheelFR->SetRelativeLocation(FVector(142.0f, 92.0f, 38.0f - BrakeSquat * 0.25f));
        TemplateWheelRL->SetRelativeLocation(FVector(-155.0f, -92.0f, 38.0f + BrakeSquat * 0.2f));
        TemplateWheelRR->SetRelativeLocation(FVector(-155.0f, 92.0f, 38.0f + BrakeSquat * 0.2f));

        TemplateWheelFL->SetRelativeRotation(FRotator(WheelSpinDegrees, SteerDegrees, 0.0f));
        TemplateWheelFR->SetRelativeRotation(FRotator(WheelSpinDegrees, SteerDegrees, 0.0f));
        TemplateWheelRL->SetRelativeRotation(FRotator(WheelSpinDegrees, 0.0f, 0.0f));
        TemplateWheelRR->SetRelativeRotation(FRotator(WheelSpinDegrees, 0.0f, 0.0f));
    }
    else
    {
        PrototypeBody->SetRelativeRotation(FRotator(BodyPitchDegrees, 0.0f, BodyRollDegrees));
        PrototypeNose->SetRelativeRotation(FRotator(BodyPitchDegrees, 0.0f, BodyRollDegrees));
        PrototypeCabin->SetRelativeRotation(FRotator(BodyPitchDegrees * 0.7f, 0.0f, BodyRollDegrees * 0.7f));
        PrototypeRearDeck->SetRelativeRotation(FRotator(BodyPitchDegrees, 0.0f, BodyRollDegrees));
        const FRotator SpinRotation(90.0f, WheelSpinDegrees, 0.0f);
        PrototypeWheelFL->SetRelativeRotation(SpinRotation + FRotator(0.0f, 0.0f, RawSteer * 14.0f));
        PrototypeWheelFR->SetRelativeRotation(SpinRotation + FRotator(0.0f, 0.0f, RawSteer * 14.0f));
        PrototypeWheelRL->SetRelativeRotation(SpinRotation);
        PrototypeWheelRR->SetRelativeRotation(SpinRotation);
    }
}

void AJCRVehiclePawn::TeleportToStart()
{
    PrototypeSpeedCms = 0.0f;
    LateralSlipCms = 0.0f;
    VerticalSpeedCms = 0.0f;
    TimeWithoutGround = 0.0f;
    bGrounded = false;
    ThrottleInput = 0.0f;
    BrakeInput = 0.0f;
    BrakeTemperature = 0.0f;
    ShiftTimer = 0.0f;
    SimulatedGear = 1;
    SimulatedRpm = 1100.0f;
    BodyRollDegrees = 0.0f;
    BodyPitchDegrees = 0.0f;
    KeyboardSteer = 0.0f;
    GamepadSteer = 0.0f;
    bLeftHeld = false;
    bRightHeld = false;
    bHandbrakeHeld = false;

    SetActorLocation(FVector(260.0f, 0.0f, 420.0f), false);
    SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));

    FHitResult GroundHit;
    if (FindNearestGround(GroundHit))
    {
        SetActorLocation(GroundHit.Location + FVector(0.0f, 0.0f, GroundClearanceCm), false);
        SetActorRotation(MakeGroundAlignedRotation(0.0f, GroundHit.Normal));
        bGrounded = true;
    }
    else
    {
        SetActorLocation(FVector(260.0f, 0.0f, 95.0f), false);
        SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
    }

    if (GetVehicleMovementComponent())
    {
        GetVehicleMovementComponent()->SetThrottleInput(0.0f);
        GetVehicleMovementComponent()->SetBrakeInput(0.0f);
        GetVehicleMovementComponent()->SetSteeringInput(0.0f);
    }
}

UStaticMeshComponent* AJCRVehiclePawn::CreatePrototypePart(const FName& Name, UStaticMesh* StaticMesh, const FVector& Location, const FVector& Scale, const FRotator& Rotation)
{
    UStaticMeshComponent* Part = CreateDefaultSubobject<UStaticMeshComponent>(Name);
    Part->SetupAttachment(GetMesh());
    Part->SetStaticMesh(StaticMesh);
    Part->SetRelativeLocation(Location);
    Part->SetRelativeScale3D(Scale);
    Part->SetRelativeRotation(Rotation);
    Part->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Part->SetCastShadow(true);
    Part->SetVisibility(false, true);
    return Part;
}

void AJCRVehiclePawn::ApplyPrototypeMaterial(UStaticMeshComponent* Component, const FName& Name, const FLinearColor& Color, float Roughness, float Metallic)
{
    if (!Component)
    {
        return;
    }

    UMaterialInstanceDynamic* Material = UMaterialInstanceDynamic::Create(UMaterial::GetDefaultMaterial(MD_Surface), this, Name);
    Material->SetVectorParameterValue(TEXT("BaseColor"), Color);
    Material->SetVectorParameterValue(TEXT("Color"), Color);
    Material->SetScalarParameterValue(TEXT("Roughness"), Roughness);
    Material->SetScalarParameterValue(TEXT("Metallic"), Metallic);
    Component->SetMaterial(0, Material);
}

void AJCRVehiclePawn::SetPrototypeVisible(bool bVisible)
{
    TArray<UStaticMeshComponent*> Parts = {
        PrototypeBody, PrototypeNose, PrototypeCabin, PrototypeRearDeck, PrototypeWing, PrototypeSplitter,
        PrototypeWheelFL, PrototypeWheelFR, PrototypeWheelRL, PrototypeWheelRR
    };

    for (UStaticMeshComponent* Part : Parts)
    {
        if (Part)
        {
            Part->SetVisibility(bVisible, true);
        }
    }
}

void AJCRVehiclePawn::SetTemplateSportsCarVisible(bool bVisible)
{
    TArray<UStaticMeshComponent*> Parts = {
        TemplateSportsCarBody, TemplateSportsCarGlass, TemplateWheelFL, TemplateWheelFR, TemplateWheelRL, TemplateWheelRR
    };

    for (UStaticMeshComponent* Part : Parts)
    {
        if (Part)
        {
            Part->SetVisibility(bVisible, true);
        }
    }
}
