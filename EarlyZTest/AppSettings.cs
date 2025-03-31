enum DiscardModes
{
    NoDiscard,
    DiscardChecker,
    DiscardNever,
}

enum DepthExportModes
{
    NoDepthExport,
    ArbitraryDepth,
    ConservativeDepthMatching,
    ConservativeDepthOpposing,
}

enum UAVWriteModes
{
    NoUAV,
    StandardUAV,
    ROV,
}

public class Settings
{
    [ExpandGroup(true)]
    public class TestConfig
    {
        bool EnableDepthWrites = false;

        bool ReverseTriangleOrder = false;

        [ShaderCompileTimeConstant(true)]
        DiscardModes DiscardMode = DiscardModes.NoDiscard;

        [ShaderCompileTimeConstant(true)]
        DepthExportModes DepthExportMode = DepthExportModes.NoDepthExport;

        [ShaderCompileTimeConstant(true)]
        UAVWriteModes UAVWriteMode = UAVWriteModes.NoUAV;

        [ShaderCompileTimeConstant(true)]
        bool ForceEarlyZ = false;
    }

    [ExpandGroup(false)]
    public class Debug
    {
        [UseAsShaderConstant(false)]
        [DisplayName("Enable VSync")]
        [HelpText("Enables or disables vertical sync during Present")]
        bool EnableVSync = true;
    }
}