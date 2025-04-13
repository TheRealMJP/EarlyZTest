# Early-Z Test
A simple D3D12 app for testing and observing Z culling behavior.

![z-tester-front-to-back](https://github.com/user-attachments/assets/c37b4712-ff46-4a2a-bf14-ae38619ae923)

This app allows you to re-create various scenarios that may or may not interact with the hardware's ability to cull pixel/fragement shaders based on early depth testing. The results are visualized as two simple colored triangles for simplicity, and a pipeline stats query is used to measure and display the number of pixel shader invocations that occurred when drawing these two triangles. This invocation count is displayed in the window, which makes it possible to determine how many pixel shader theads were culled before they could be executed.

## Settings

* `Enable Depth Writes` - enables or disables depth writes in the depth/stencil state of the PSO (depth testing is always enabled)
* `Reverse Triangle Order` - if disabled, the two triangles are drawn back-to-front. If enabled, the triangles are drawn front-to-back.
* `Discard Mode` - controls whether `discard` is present in the pixel shader, and how it's used:
  * `NoDiscard` - no `discard` is present
  * `DiscardChecker` - `discard` is used to kill pixels in a coarse 32x32 checkerboard pattern
  * `DiscardNever` - a `discard` is put in a branch that is never taken
* `Depth Export Mode` - controls how the pixel shader outputs/exports a manual depth value
  * `NoDepthExport` - no depth is output by the shader
  * `ArbitraryDepth` - the depth value is fully overriden by outputting `SV_Depth`
  * `ConservativeDepthMatching` - a conservative depth output is used where the inequality matches the depth test inequality (`SV_DepthLessEqual`)
  * `ConservativeDepthOpposing` - a conservative depth output is used where the inequality opposes the depth test inequality (`SV_DepthGreaterEqual`)
* `UAV Write Mode` - controls what sort of UAV write occurrs from the pixel shader
  * `NoUAV` - no UAV writes are present in the pixel shader. The pixel shader outputs to `SV_Target0`.
  * `StandardUAV` - the pixel shader outputs its color to a `RWTexture2D<float4`
  * `ROV` - the pixel shader outputs its color to a `RasterizerOrderedTexture2D<float4>`
* `Force Early Z` - if enabled, the pixel shader forces the hardware to perform all depth tests before the pixel shader executes using the `[earlydepthstencil]` attribute

## How To Build and Run

Open `EarlyZTest\EarlyZTest.sln`, build the solution, and then run it.
