#include "RenderingSystem.hpp"
#include "Renderer.hpp"


namespace proto
{
	RenderingSystem::RenderingSystem(Renderer* ren)
		: _renderer(ren)
	{
	}
	
	void RenderingSystem::Update()
	{
		
	}

	void RenderingSystem::RenderUIDrawCalls()
	{
		_renderer->PushDrawCallRange(_uiDrawCalls);
		_uiDrawCalls = std::span<DrawCall>{};
	}

	void RenderingSystem::SetUIDrawCalls(std::span<DrawCall> calls)
	{
		_uiDrawCalls = calls;
	}
}
