#pragma once

#include <PCH.h>
#include <Settings.h>
#include <Graphics\GraphicsTypes.h>

using namespace SampleFramework12;

enum class DiscardModes
{
    NoDiscard = 0,
    DiscardChecker = 1,
    DiscardNever = 2,

    NumValues
};

extern const char* DiscardModesLabels[uint32(DiscardModes::NumValues)];

extern const DiscardModes DiscardModesValues[uint32(DiscardModes::NumValues)];

typedef EnumSettingT<DiscardModes> DiscardModesSetting;

enum class DepthExportModes
{
    NoDepthExport = 0,
    ArbitraryDepth = 1,
    ConservativeDepthMatching = 2,
    ConservativeDepthOpposing = 3,

    NumValues
};

extern const char* DepthExportModesLabels[uint32(DepthExportModes::NumValues)];

extern const DepthExportModes DepthExportModesValues[uint32(DepthExportModes::NumValues)];

typedef EnumSettingT<DepthExportModes> DepthExportModesSetting;

enum class UAVWriteModes
{
    NoUAV = 0,
    StandardUAV = 1,
    ROV = 2,

    NumValues
};

extern const char* UAVWriteModesLabels[uint32(UAVWriteModes::NumValues)];

extern const UAVWriteModes UAVWriteModesValues[uint32(UAVWriteModes::NumValues)];

typedef EnumSettingT<UAVWriteModes> UAVWriteModesSetting;

namespace AppSettings
{

    extern BoolSetting EnableDepthWrites;
    extern BoolSetting ReverseTriangleOrder;
    extern DiscardModesSetting DiscardMode;
    extern DepthExportModesSetting DepthExportMode;
    extern UAVWriteModesSetting UAVWriteMode;
    extern BoolSetting ForceEarlyZ;
    extern BoolSetting ClearDepthToZero;
    extern BoolSetting BarrierBetweenDraws;
    extern BoolSetting EnableVSync;

    struct AppSettingsCBuffer
    {
        bool32 EnableDepthWrites;
        bool32 ReverseTriangleOrder;
        bool32 ClearDepthToZero;
        bool32 BarrierBetweenDraws;
    };

    extern ConstantBuffer CBuffer;
    const extern uint32 CBufferRegister;

    void Initialize();
    void Shutdown();
    void Update(uint32 displayWidth, uint32 displayHeight, const Float4x4& viewMatrix);
    void UpdateCBuffer();
    void BindCBufferGfx(ID3D12GraphicsCommandList* cmdList, uint32 rootParameter);
    void BindCBufferCompute(ID3D12GraphicsCommandList* cmdList, uint32 rootParameter);
    void GetShaderCompileOptions(CompileOptions& opts);
    bool ShaderCompileOptionsChanged();
};

// ================================================================================================

namespace AppSettings
{
}