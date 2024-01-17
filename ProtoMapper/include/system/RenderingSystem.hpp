#ifndef PROTO_RENDERINGSYSTEM_HPP
#define PROTO_RENDERINGSYSTEM_HPP

#include "System.hpp"

namespace proto
{

	class RenderingSystem : public System
	{

	public:
		RenderingSystem() = default;

		bool IsActive() const override;
		void Update(entt::registry& registry, float dt) override;

	};

}

#endif // !PROTO_RENDERINGSYSTEM_HPP
