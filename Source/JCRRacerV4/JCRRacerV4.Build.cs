using UnrealBuildTool;

public class JCRRacerV4 : ModuleRules
{
    public JCRRacerV4(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "ChaosVehicles",
            "UMG",
            "Slate",
            "SlateCore",
            "Niagara",
            "AudioMixer"
        });
    }
}
