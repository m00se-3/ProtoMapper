#ifndef PROTO_RENDERINGSYSTEM_HPP
#define PROTO_RENDERINGSYSTEM_HPP

#include <vector>
#include <span>

#include "System.hpp"
#include "Vertex.hpp"
#include "Renderer.hpp"

namespace proto
{

	class RenderingSystem : public System
	{

	public:
		RenderingSystem(Renderer* ren);

		bool IsActive() const override;
		void Update(entt::registry& registry, [[maybe_unused]]float dt) override;
		void SetUIDrawCalls(const std::span<DrawCall>& calls);

	private:

		Renderer* _renderer = nullptr;

		Buffer<Vertex2D> _sceneBuffer;
		std::span<DrawCall> _uiDrawCalls;
	};

}

#endif // !PROTO_RENDERINGSYSTEM_HPP
