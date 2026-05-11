using UnrealBuildTool;
using System.Collections.Generic;

public class JCRRacerV4Target : TargetRules
{
    public JCRRacerV4Target(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V6;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("JCRRacerV4");
    }
}
