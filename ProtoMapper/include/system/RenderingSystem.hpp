#ifndef PROTO_RENDERINGSYSTEM_HPP
#define PROTO_RENDERINGSYSTEM_HPP

#include <span>

#include "Vertex.hpp"
#include "Renderer.hpp"

namespace proto
{

	class RenderingSystem
	{

	public:
		RenderingSystem(Renderer* ren);

		[[nodiscard]] bool IsActive() const { return _renderer != nullptr; }
		void Update();
		void SetUIDrawCalls(std::span<DrawCall> calls);
		void RenderUIDrawCalls();

	private:

		Renderer* _renderer = nullptr;

		Buffer<Vertex2D> _sceneBuffer;
		std::span<DrawCall> _uiDrawCalls;
	};

}

#endif // !PROTO_RENDERINGSYSTEM_HPP
