//=================================================================================================
//
//  D3D12 Memory Pool Performance Test
//  by MJP
//  https://therealmjp.github.io/
//
//  All code and content licensed under the MIT license
//
//=================================================================================================

#include <PCH.h>

#include <Window.h>
#include <Input.h>
#include <Utility.h>
#include <Graphics/SwapChain.h>
#include <Graphics/ShaderCompilation.h>
#include <Graphics/Profiler.h>
#include <Graphics/DX12.h>
#include <Graphics/DX12_Helpers.h>
#include <ImGui/ImGui.h>
#include <ImGuiHelper.h>

#include "EarlyZTest.h"
#include "SharedTypes.h"
#include "AppSettings.h"

using namespace SampleFramework12;

EarlyZTest::EarlyZTest(const wchar* cmdLine) : App(L"Early-Z Test", cmdLine)
{
    swapChain.SetFormat(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
}

void EarlyZTest::BeforeReset()
{
}

void EarlyZTest::AfterReset()
{
    uint32 width = swapChain.Width();
    uint32 height = swapChain.Height();

    mainTarget.Initialize({
        .Width = width,
        .Height = height,
        .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .MSAASamples = 1,
        .ArraySize = 1,
        .CreateUAV = true,
        .CreateRTV = true,
        .Name = L"Main Target",
    });

    depthBuffer.Initialize({
        .Width = width,
        .Height = height,
        .Format = DXGI_FORMAT_D32_FLOAT,
        .MSAASamples = 1,
        .Name = L"Main Depth Buffer",
    });
}

void EarlyZTest::Initialize()
{
    testVS = CompileFromFile(L"EarlyZTest.hlsl", "VSMain", ShaderType::Vertex);
    testPS = CompileFromFile(L"EarlyZTest.hlsl", "PSMain", ShaderType::Pixel);

    D3D12_QUERY_HEAP_DESC heapDesc = { };
    heapDesc.Count = 1;
    heapDesc.NodeMask = 0;
    heapDesc.Type = D3D12_QUERY_HEAP_TYPE_PIPELINE_STATISTICS;
    DX12::Device->CreateQueryHeap(&heapDesc, IID_PPV_ARGS(&queryHeap));

    for(uint32 i = 0; i < DX12::RenderLatency; ++i)
    {
        queryReadbackBuffers[i].Initialize(sizeof(D3D12_QUERY_DATA_PIPELINE_STATISTICS));
        queryReadbackBuffers[i].Resource->SetName(L"Query Readback Buffer");
    }
}

void EarlyZTest::Shutdown()
{
    mainTarget.Shutdown();
    depthBuffer.Shutdown();
    DX12::Release(queryHeap);
    for (ReadbackBuffer& buffer : queryReadbackBuffers)
        buffer.Shutdown();
}

void EarlyZTest::CreatePSOs()
{
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.pRootSignature = DX12::UniversalRootSignature;
        psoDesc.VS = testVS.ByteCode();
        psoDesc.PS = testPS.ByteCode();
        psoDesc.RasterizerState = DX12::GetRasterizerState(RasterizerState::NoCull);
        psoDesc.BlendState = DX12::GetBlendState(BlendState::Disabled);
        psoDesc.DepthStencilState = DX12::GetDepthState(DepthState::Enabled);
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = mainTarget.Format();
        psoDesc.DSVFormat = depthBuffer.DSVFormat;
        psoDesc.SampleDesc.Count = 1;
        psoDesc.SampleDesc.Quality = 0;
        DXCall(DX12::Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&testPSO)));

        psoDesc.DepthStencilState = DX12::GetDepthState(DepthState::WritesEnabled);
        DXCall(DX12::Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&testDepthWritePSO)));
    }
}

void EarlyZTest::DestroyPSOs()
{
    DX12::DeferredRelease(testPSO);
    DX12::DeferredRelease(testDepthWritePSO);
}

void EarlyZTest::Update(const Timer& timer)
{
    CPUProfileBlock cpuProfileBlock("Update");

    // Toggle VSYNC
    swapChain.SetVSYNCEnabled(AppSettings::EnableVSync ? true : false);
}

void EarlyZTest::Render(const Timer& timer)
{
    ID3D12GraphicsCommandList10* cmdList = DX12::CmdList;

    CPUProfileBlock cpuProfileBlock("Render");
    ProfileBlock gpuProfileBlock(cmdList, "Render Total");

    {
        // Transition render targets back to a writable state
        BarrierBatchBuilder barrierBuilder;
        barrierBuilder.Add(mainTarget.RTWritableBarrier({ .FirstAccess = true }));
        barrierBuilder.Add(depthBuffer.DepthWritableBarrier({ .FirstAccess = true }));
        DX12::Barrier(cmdList, barrierBuilder.Build());
    }

    {
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[1] = { mainTarget.RTV };
        cmdList->OMSetRenderTargets(1, rtvHandles, false, &depthBuffer.DSV);

        float clearColor[4] = { 0.2f, 0.4f, 0.8f, 1.0f };
        cmdList->ClearRenderTargetView(rtvHandles[0], clearColor, 0, nullptr);
        cmdList->ClearDepthStencilView(depthBuffer.DSV, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, AppSettings::ClearDepthToZero ? 0.0f : 1.0f, 0, 0, nullptr);
    }

    const bool useUAV = AppSettings::UAVWriteMode != UAVWriteModes::NoUAV;
    if (useUAV)
    {
        BarrierBatchBuilder barrierBuilder;
        barrierBuilder.Add(mainTarget.UAVWritableBarrier({ .SyncBefore = D3D12_BARRIER_SYNC_RENDER_TARGET, .AccessBefore = D3D12_BARRIER_ACCESS_RENDER_TARGET, .LayoutBefore = D3D12_BARRIER_LAYOUT_RENDER_TARGET }));
        DX12::Barrier(cmdList, barrierBuilder.Build());

        cmdList->OMSetRenderTargets(0, nullptr, false, &depthBuffer.DSV);
    }

    DX12::SetViewport(cmdList, swapChain.Width(), swapChain.Height());

    cmdList->SetPipelineState(AppSettings::EnableDepthWrites ? testDepthWritePSO : testPSO);
    cmdList->SetGraphicsRootSignature(DX12::UniversalRootSignature);
    cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    AppSettings::BindCBufferGfx(cmdList, URS_AppSettings);

    TestConstants testConstants =
    {
        .OutputTexture = useUAV ? mainTarget.UAV : InvalidDescriptorIndex,
    };
    DX12::BindTempConstantBuffer(cmdList, testConstants, URS_ConstantBuffers + 0, CmdListMode::Graphics);

    cmdList->BeginQuery(queryHeap, D3D12_QUERY_TYPE_PIPELINE_STATISTICS, 0);

    cmdList->DrawInstanced(3, 2, 0, 0);

    cmdList->EndQuery(queryHeap, D3D12_QUERY_TYPE_PIPELINE_STATISTICS, 0);
    cmdList->ResolveQueryData(queryHeap, D3D12_QUERY_TYPE_PIPELINE_STATISTICS, 0, 1, queryReadbackBuffers[DX12::CurrFrameIdx].Resource, 0);

    // Copy to the back buffer
    {
        BarrierBatchBuilder barrierBuilder;
        if (useUAV)
            barrierBuilder.Add(mainTarget.UAVToShaderReadableBarrier({ }));
        else
            barrierBuilder.Add(mainTarget.RTToShaderReadableBarrier({ }));
        DX12::Barrier(cmdList, barrierBuilder.Build());
    }

    {
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[1] = { swapChain.BackBuffer().RTV };
        cmdList->OMSetRenderTargets(1, rtvHandles, false, nullptr);
    }

    Float2 viewportSize;
    viewportSize.x = float(swapChain.Width());
    viewportSize.y = float(swapChain.Height());
    spriteRenderer.Begin(cmdList, viewportSize, SpriteFilterMode::Point, SpriteBlendMode::Opaque);
    spriteRenderer.Render(cmdList, &mainTarget.Texture, SpriteTransform());
    spriteRenderer.End();

    const D3D12_QUERY_DATA_PIPELINE_STATISTICS* pipelineStats = queryReadbackBuffers[DX12::CurrFrameIdx].Map<D3D12_QUERY_DATA_PIPELINE_STATISTICS>();

    std::string statsString = MakeString("Num PS Invocations: %llu", pipelineStats->PSInvocations);
    Float2 textSize = ToFloat2(ImGui::CalcTextSize(statsString.c_str()));
    Float2 windowSize = textSize * 1.05f;

    Float2 windowPos = Float2((viewportSize.x * 0.5f) - (windowSize.x * 0.5f), viewportSize.y - 100.0f - windowSize.y);
    ImGui::SetNextWindowPos(ToImVec2(windowPos), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ToImVec2(windowSize), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin("Stats Window", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs |
                                          ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
                                          ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                          ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);
    ImGui::GetWindowDrawList()->AddText(ToImVec2(windowPos + (windowSize - textSize) * 0.5f), ImColor(1.0f, 1.0f, 1.0f, 1.0f), statsString.c_str());
    ImGui::PopStyleVar();
    ImGui::End();
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    EarlyZTest app(lpCmdLine);
    app.Run();
}
