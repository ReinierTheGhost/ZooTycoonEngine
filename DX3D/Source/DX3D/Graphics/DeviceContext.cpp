#include <DX3D/Graphics/DeviceContext.h>
#include <DX3D/Graphics/SwapChain.h>

dx3d::DeviceContext::DeviceContext(const GraphicsResourceDesc& gDesc): GraphicsResource(gDesc)
{
	DX3DGraphicsLogThrowOnFail(m_device.CreateDeferredContext(0, &m_context),
		"CreateDeferredContext failed.");
}

void dx3d::DeviceContext::clearAndSetBackBuffer(const SwapChain& swapChain, const Vec4 color)
{
	f32 fColor[] = { color.r,color.g,color.b,color.a };
	auto rtv = swapChain.m_rtv.Get();
	m_context->ClearRenderTargetView(rtv, fColor);
	m_context->OMGetRenderTargets(1, &rtv, nullptr);
}
