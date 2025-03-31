//=================================================================================================
//
//  D3D12 Memory Pool Performance Test
//  by MJP
//  https://therealmjp.github.io/
//
//  All code and content licensed under the MIT license
//
//=================================================================================================

#pragma once

#include <PCH.h>

#include <App.h>
#include <Graphics/GraphicsTypes.h>
#include "AppSettings.h"

using namespace SampleFramework12;

class EarlyZTest : public App
{

protected:

    CompiledShaderPtr testVS;
    CompiledShaderPtr testPS;
    ID3D12PipelineState* testPSO = nullptr;
    ID3D12PipelineState* testDepthWritePSO = nullptr;

    RenderTexture mainTarget;
    DepthBuffer depthBuffer;

    ID3D12QueryHeap* queryHeap = nullptr;
    ReadbackBuffer queryReadbackBuffers[DX12::RenderLatency];

    virtual void Initialize() override;
    virtual void Shutdown() override;

    virtual void Render(const Timer& timer) override;
    virtual void Update(const Timer& timer) override;

    virtual void BeforeReset() override;
    virtual void AfterReset() override;

    virtual void CreatePSOs() override;
    virtual void DestroyPSOs() override;

public:

    EarlyZTest(const wchar* cmdLine);
};
