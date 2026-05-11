using UnrealBuildTool;
using System.Collections.Generic;

public class JCRRacerV4EditorTarget : TargetRules
{
    public JCRRacerV4EditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V6;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("JCRRacerV4");
    }
}
