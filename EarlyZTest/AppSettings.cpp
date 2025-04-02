#include <PCH.h>
#include <Graphics\ShaderCompilation.h>
#include "AppSettings.h"

using namespace SampleFramework12;

const char* DiscardModesLabels[uint32(DiscardModes::NumValues)] =
{
    "NoDiscard",
    "DiscardChecker",
    "DiscardNever",
};

const DiscardModes DiscardModesValues[uint32(DiscardModes::NumValues)] =
{
    DiscardModes::NoDiscard,
    DiscardModes::DiscardChecker,
    DiscardModes::DiscardNever,
};

const char* DepthExportModesLabels[uint32(DepthExportModes::NumValues)] =
{
    "NoDepthExport",
    "ArbitraryDepth",
    "ConservativeDepthMatching",
    "ConservativeDepthOpposing",
};

const DepthExportModes DepthExportModesValues[uint32(DepthExportModes::NumValues)] =
{
    DepthExportModes::NoDepthExport,
    DepthExportModes::ArbitraryDepth,
    DepthExportModes::ConservativeDepthMatching,
    DepthExportModes::ConservativeDepthOpposing,
};

const char* UAVWriteModesLabels[uint32(UAVWriteModes::NumValues)] =
{
    "NoUAV",
    "StandardUAV",
    "ROV",
};

const UAVWriteModes UAVWriteModesValues[uint32(UAVWriteModes::NumValues)] =
{
    UAVWriteModes::NoUAV,
    UAVWriteModes::StandardUAV,
    UAVWriteModes::ROV,
};

namespace AppSettings
{
    static SettingsContainer Settings;

    BoolSetting EnableDepthWrites;
    BoolSetting ReverseTriangleOrder;
    DiscardModesSetting DiscardMode;
    DepthExportModesSetting DepthExportMode;
    UAVWriteModesSetting UAVWriteMode;
    BoolSetting ForceEarlyZ;
    BoolSetting ClearDepthToZero;
    BoolSetting BarrierBetweenDraws;
    BoolSetting EnableVSync;

    ConstantBuffer CBuffer;
    const uint32 CBufferRegister = 12;

    void Initialize()
    {

        Settings.Initialize(2);

        Settings.AddGroup("Test Config", true);

        Settings.AddGroup("Debug", false);

        EnableDepthWrites.Initialize("EnableDepthWrites", "Test Config", "Enable Depth Writes", "", false);
        Settings.AddSetting(&EnableDepthWrites);

        ReverseTriangleOrder.Initialize("ReverseTriangleOrder", "Test Config", "Reverse Triangle Order", "", false);
        Settings.AddSetting(&ReverseTriangleOrder);

        DiscardMode.Initialize("DiscardMode", "Test Config", "Discard Mode", "", DiscardModes::NoDiscard, 3, DiscardModesLabels);
        Settings.AddSetting(&DiscardMode);

        DepthExportMode.Initialize("DepthExportMode", "Test Config", "Depth Export Mode", "", DepthExportModes::NoDepthExport, 4, DepthExportModesLabels);
        Settings.AddSetting(&DepthExportMode);

        UAVWriteMode.Initialize("UAVWriteMode", "Test Config", "UAVWrite Mode", "", UAVWriteModes::NoUAV, 3, UAVWriteModesLabels);
        Settings.AddSetting(&UAVWriteMode);

        ForceEarlyZ.Initialize("ForceEarlyZ", "Test Config", "Force Early Z", "", false);
        Settings.AddSetting(&ForceEarlyZ);

        ClearDepthToZero.Initialize("ClearDepthToZero", "Test Config", "Clear Depth To Zero", "", false);
        Settings.AddSetting(&ClearDepthToZero);

        BarrierBetweenDraws.Initialize("BarrierBetweenDraws", "Test Config", "Barrier Between Draws", "", false);
        Settings.AddSetting(&BarrierBetweenDraws);

        EnableVSync.Initialize("EnableVSync", "Debug", "Enable VSync", "Enables or disables vertical sync during Present", true);
        Settings.AddSetting(&EnableVSync);

        ConstantBufferInit cbInit;
        cbInit.Size = sizeof(AppSettingsCBuffer);
        cbInit.Dynamic = true;
        cbInit.Name = L"AppSettings Constant Buffer";
        CBuffer.Initialize(cbInit);
    }

    void Update(uint32 displayWidth, uint32 displayHeight, const Float4x4& viewMatrix)
    {
        Settings.Update(displayWidth, displayHeight, viewMatrix);

    }

    void UpdateCBuffer()
    {
        AppSettingsCBuffer cbData;
        cbData.EnableDepthWrites = EnableDepthWrites;
        cbData.ReverseTriangleOrder = ReverseTriangleOrder;
        cbData.ClearDepthToZero = ClearDepthToZero;
        cbData.BarrierBetweenDraws = BarrierBetweenDraws;

        CBuffer.MapAndSetData(cbData);
    }

    void BindCBufferGfx(ID3D12GraphicsCommandList* cmdList, uint32 rootParameter)
    {
        CBuffer.SetAsGfxRootParameter(cmdList, rootParameter);
    }

    void BindCBufferCompute(ID3D12GraphicsCommandList* cmdList, uint32 rootParameter)
    {
        CBuffer.SetAsComputeRootParameter(cmdList, rootParameter);
    }

    void GetShaderCompileOptions(CompileOptions& opts)
    {
        opts.Add("DiscardMode_", DiscardMode.Value());
        opts.Add("DepthExportMode_", DepthExportMode.Value());
        opts.Add("UAVWriteMode_", UAVWriteMode.Value());
        opts.Add("ForceEarlyZ_", ForceEarlyZ.Value());
    }

    bool ShaderCompileOptionsChanged()
    {
        bool changed = false;
        changed = changed || DiscardMode.Changed();
        changed = changed || DepthExportMode.Changed();
        changed = changed || UAVWriteMode.Changed();
        changed = changed || ForceEarlyZ.Changed();
        return changed;
    }

    void Shutdown()
    {
        CBuffer.Shutdown();
    }
}

// ================================================================================================

namespace AppSettings
{
}