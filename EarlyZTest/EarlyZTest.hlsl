#include <ShaderDebug.hlsl>
#include "AppSettings.hlsl"
#include "SharedTypes.h"

ConstantBuffer<TestConstants> CB : register(b0);

struct VSOutput
{
    centroid float4 Position : SV_Position;
    float4 Color : COLOR;
    uint TriIndex : TRIINDEX;
};

VSOutput VSMain(in uint vertexIndex : SV_VertexID)
{
    uint triangleIndex = CB.DrawIndex;
    if (AppSettings.ReverseTriangleOrder)
        triangleIndex = (triangleIndex + 1) % 2;

    float4 position = float4(-0.5f, -0.5f, 1.0f, 1.0f);
    if (vertexIndex == 1)
        position.xy = float2(0.0f, 0.75f);
    else if(vertexIndex == 2)
        position.xy = float2(0.5f, -0.5f);
    if (triangleIndex == 1)
    {
        position.y *= -1.0f;
        position.z = 0.5f;
    }

    VSOutput output;
    output.Position = position;
    output.Color = triangleIndex == 1 ? float4(0.1f, 0.9f, 0.1f, 1.0f) : float4(0.9f, 0.1f, 0.1f, 1.0f);
    output.TriIndex = triangleIndex;
    return output;
}

#if DepthExportMode_ != NoDepthExport_ || UAVWriteMode_ == NoUAV_
    #define HasPSOutput_ 1

    struct PSOutput
    {
    #if DepthExportMode_ == ArbitraryDepth_
        float Depth : SV_Depth;
    #elif DepthExportMode_ == ConservativeDepthMatching_
        float Depth : SV_DepthLessEqual;
    #elif DepthExportMode_ == ConservativeDepthOpposing_
        float Depth : SV_DepthGreaterEqual;
    #endif

    #if UAVWriteMode_ == NoUAV_
        float4 Color : SV_Target0;
    #endif
    };
#else
    #define HasPSOutput_ 0
    #define PSOutput void
#endif

#if ForceEarlyZ_
    [earlydepthstencil]
#endif
PSOutput PSMain(in VSOutput vsOutput)
{
#if DiscardMode_ == DiscardChecker_
    uint2 checker = (uint2(vsOutput.Position.xy) / 32) % 2;
    if ((checker.x ^ checker.y) == vsOutput.TriIndex)
        discard;
#elif DiscardMode_ == DiscardNever_
    if (vsOutput.Color.x < 0.0f)
        discard;
#endif

#if UAVWriteMode_ != NoUAV_
    #if UAVWriteMode_ == StandardUAV_
        RWTexture2D<float4> outputTexture = ResourceDescriptorHeap[CB.OutputTexture];
    #elif UAVWriteMode_ == ROV_
        RasterizerOrderedTexture2D<float4> outputTexture = ResourceDescriptorHeap[CB.OutputTexture];
    #endif

    outputTexture[uint2(vsOutput.Position.xy)] = vsOutput.Color;
#endif

#if HasPSOutput_
    PSOutput psOutput = (PSOutput)0;

    #if DepthExportMode_ != NoDepthExport_
        psOutput.Depth = vsOutput.Position.z;
    #endif

    #if UAVWriteMode_ == NoUAV_
        psOutput.Color = vsOutput.Color;
    #endif

    return psOutput;
#endif
}