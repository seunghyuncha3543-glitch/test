#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "InputActionValue.h"
#include "JCRVehiclePawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UInputMappingContext;
class UNiagaraComponent;
class UPointLightComponent;
class UChaosWheeledVehicleMovementComponent;
class UStaticMeshComponent;
class UMaterialInterface;
class UJCREngineSynth;

UCLASS(Blueprintable)
class JCRRACERV4_API AJCRVehiclePawn : public AWheeledVehiclePawn
{
    GENERATED_BODY()

public:
    AJCRVehiclePawn();

    virtual void Tick(float DeltaSeconds) override;
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    UFUNCTION(BlueprintPure, Category = "Vehicle|Telemetry")
    float GetSpeedKmh() const;

    UFUNCTION(BlueprintPure, Category = "Vehicle|Telemetry")
    float GetDisplayRpm() const;

    UFUNCTION(BlueprintPure, Category = "Vehicle|Telemetry")
    int32 GetDisplayGear() const;

    UFUNCTION(BlueprintPure, Category = "Vehicle|Assists")
    bool IsAbsEnabled() const { return bAbsEnabled; }

    UFUNCTION(BlueprintPure, Category = "Vehicle|Assists")
    bool IsTcsEnabled() const { return bTcsEnabled; }

    UFUNCTION(BlueprintPure, Category = "Vehicle|Assists")
    bool IsSportModeEnabled() const { return bSportModeEnabled; }

    UFUNCTION(BlueprintPure, Category = "Vehicle|Assists")
    bool IsHandbrakeActive() const { return bHandbrakeHeld; }

    UFUNCTION(BlueprintPure, Category = "Vehicle|Telemetry")
    float GetBrakeTemperature() const { return BrakeTemperature; }

    UFUNCTION(BlueprintPure, Category = "Vehicle|Camera")
    int32 GetCameraMode() const { return CameraMode; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<USpringArmComponent> ChaseArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<UCameraComponent> ChaseCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Prototype")
    TObjectPtr<UStaticMeshComponent> PrototypeBody;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Prototype")
    TObjectPtr<UStaticMeshComponent> PrototypeNose;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Prototype")
    TObjectPtr<UStaticMeshComponent> PrototypeCabin;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Prototype")
    TObjectPtr<UStaticMeshComponent> PrototypeRearDeck;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Prototype")
    TObjectPtr<UStaticMeshComponent> PrototypeWing;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Prototype")
    TObjectPtr<UStaticMeshComponent> PrototypeSplitter;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Prototype")
    TObjectPtr<UStaticMeshComponent> PrototypeWheelFL;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Prototype")
    TObjectPtr<UStaticMeshComponent> PrototypeWheelFR;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Prototype")
    TObjectPtr<UStaticMeshComponent> PrototypeWheelRL;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Prototype")
    TObjectPtr<UStaticMeshComponent> PrototypeWheelRR;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Visual")
    TObjectPtr<UStaticMeshComponent> TemplateSportsCarBody;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Visual")
    TObjectPtr<UStaticMeshComponent> TemplateSportsCarGlass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Visual")
    TObjectPtr<UStaticMeshComponent> TemplateWheelFL;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Visual")
    TObjectPtr<UStaticMeshComponent> TemplateWheelFR;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Visual")
    TObjectPtr<UStaticMeshComponent> TemplateWheelRL;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Visual")
    TObjectPtr<UStaticMeshComponent> TemplateWheelRR;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects")
    TObjectPtr<UPointLightComponent> LeftHeadlight;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects")
    TObjectPtr<UPointLightComponent> RightHeadlight;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects")
    TObjectPtr<UPointLightComponent> LeftBrakeLight;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects")
    TObjectPtr<UPointLightComponent> RightBrakeLight;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects")
    TObjectPtr<UNiagaraComponent> DustTrail;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects")
    TObjectPtr<UNiagaraComponent> ExhaustEffect;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
    TObjectPtr<UJCREngineSynth> EngineSynth;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> DrivingMappingContext;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> ThrottleAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> BrakeAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> SteerLeftAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> SteerRightAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> SteerAxisAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> ToggleAbsAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> ToggleTcsAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> ResetAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> HandbrakeAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> SportModeAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> CameraModeAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Setup")
    float VehicleMassKg = 1650.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Setup")
    float MaxTorqueNm = 610.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Setup", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float FrontBrakeBias = 0.62f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Assists")
    bool bAbsEnabled = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Assists")
    bool bTcsEnabled = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float BaseFov = 70.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float MaxFov = 92.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Steering")
    float HighSpeedSteeringScale = 0.38f;

private:
    void ConfigureRuntimeInput();
    void ConfigureVehicle();
    void ApplyThrottle(const FInputActionValue& Value);
    void ApplyBrake(const FInputActionValue& Value);
    void ApplySteerLeft(const FInputActionValue& Value);
    void ApplySteerRight(const FInputActionValue& Value);
    void ApplySteerAxis(const FInputActionValue& Value);
    void StopSteerLeft(const FInputActionValue& Value);
    void StopSteerRight(const FInputActionValue& Value);
    void ToggleAbs(const FInputActionValue& Value);
    void ToggleTcs(const FInputActionValue& Value);
    void ResetVehicle(const FInputActionValue& Value);
    void ApplyHandbrake(const FInputActionValue& Value);
    void ToggleSportMode(const FInputActionValue& Value);
    void CycleCameraMode(const FInputActionValue& Value);
    void RecomputeSteering();
    void UpdateCamera(float DeltaSeconds);
    void UpdateLightsAndEffects();
    void UpdatePrototypeFallback(float DeltaSeconds);
    bool TraceGround(FHitResult& OutHit, const FVector& FromLocation) const;
    bool FindNearestGround(FHitResult& OutHit) const;
    FVector FindNearestCoursePoint(const FVector& Location, float& OutDistanceToCourse) const;
    void StabilizeOnCourse(float DeltaSeconds);
    FRotator MakeGroundAlignedRotation(float YawDegrees, const FVector& GroundNormal) const;
    void UpdateWheelVisuals(float RawSteer, float DeltaSeconds);
    void TeleportToStart();
    bool IsUsingPrototypeFallback() const;
    void SetPrototypeVisible(bool bVisible);
    void SetTemplateSportsCarVisible(bool bVisible);
    UStaticMeshComponent* CreatePrototypePart(const FName& Name, UStaticMesh* StaticMesh, const FVector& Location, const FVector& Scale, const FRotator& Rotation);
    void ApplyPrototypeMaterial(UStaticMeshComponent* Component, const FName& Name, const FLinearColor& Color, float Roughness = 0.35f, float Metallic = 0.0f);

    float ThrottleInput = 0.0f;
    float BrakeInput = 0.0f;
    float KeyboardSteer = 0.0f;
    float GamepadSteer = 0.0f;
    float PrototypeSpeedCms = 0.0f;
    float LateralSlipCms = 0.0f;
    float VerticalSpeedCms = 0.0f;
    float GroundClearanceCm = 48.0f;
    float WheelSpinDegrees = 0.0f;
    float SimulatedRpm = 1100.0f;
    float ShiftTimer = 0.0f;
    float BrakeTemperature = 0.0f;
    float BodyRollDegrees = 0.0f;
    float BodyPitchDegrees = 0.0f;
    bool bGrounded = false;
    bool bTemplateSportsCarVisualAvailable = false;
    float TimeWithoutGround = 0.0f;
    bool bLeftHeld = false;
    bool bRightHeld = false;
    bool bHandbrakeHeld = false;
    bool bSportModeEnabled = false;
    int32 CameraMode = 0;
    int32 SimulatedGear = 1;
};
