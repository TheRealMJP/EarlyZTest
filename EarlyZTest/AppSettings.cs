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
        [HelpText("enables or disables depth writes in the depth/stencil state of the PSO (depth testing is always enabled)")]
        bool EnableDepthWrites = false;

        [HelpText("if disabled, the two triangles are drawn back-to-front. If enabled, the triangles are drawn front-to-back")]
        bool ReverseTriangleOrder = false;

        [HelpText("controls whether discard is present in the pixel shader, and how it's used")]
        [ShaderCompileTimeConstant(true)]
        DiscardModes DiscardMode = DiscardModes.NoDiscard;

        [HelpText("controls how the pixel shader outputs/exports a manual depth value")]
        [ShaderCompileTimeConstant(true)]
        DepthExportModes DepthExportMode = DepthExportModes.NoDepthExport;

        [HelpText("controls what sort of UAV write occurrs from the pixel shader ")]
        [ShaderCompileTimeConstant(true)]
        UAVWriteModes UAVWriteMode = UAVWriteModes.NoUAV;

        [HelpText("if enabled, the pixel shader forces the hardware to perform all depth tests before the pixel shader executes using the [earlydepthstencil] attribute")]
        [ShaderCompileTimeConstant(true)]
        bool ForceEarlyZ = false;

        [HelpText("clears the depth buffer to 0.0 instead of 1.0 before drawing the triangles, causing all drawn pixels to fail the depth test")]
        bool ClearDepthToZero = false;

        [HelpText("issues a global memory barrier between the two triangle draws to force a stall + flush")]
        bool BarrierBetweenDraws = false;
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