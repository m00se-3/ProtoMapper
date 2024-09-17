#ifndef PROTO_RENDERINGSYSTEM_HPP
#define PROTO_RENDERINGSYSTEM_HPP

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

		[[nodiscard]] bool IsActive() const override { return _renderer != nullptr; }
		void Update(float dt) override;
		void SetUIDrawCalls(std::span<DrawCall> calls);

	private:

		Renderer* _renderer = nullptr;

		Buffer<Vertex2D> _sceneBuffer;
		std::span<DrawCall> _uiDrawCalls;
	};

}

#endif // !PROTO_RENDERINGSYSTEM_HPP
