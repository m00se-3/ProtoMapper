#include "RenderingSystem.hpp"


namespace proto
{
	RenderingSystem::RenderingSystem(Renderer* ren)
		: _renderer(ren)
	{
	}
	
	bool RenderingSystem::IsActive() const
	{
		return _renderer != nullptr;
	}

	void RenderingSystem::Update(entt::registry& registry, float dt)
	{
		_renderer->PushDrawCallRange(_uiDrawCalls);
	}

	void RenderingSystem::SetUIDrawCalls(const std::span<DrawCall>& calls)
	{
		_uiDrawCalls = calls;
	}
}