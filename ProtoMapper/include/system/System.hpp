#ifndef PROTO_SYSTEM_HPP
#define PROTO_SYSTEM_HPP

#include "entt/entt.hpp"

namespace proto
{
	/*
		An abstract class for implementing application subsystems
	*/
	class System
	{

	public:
		System() = default;
		System(const System&) = default;
		System(System&&) = default;
		System& operator=(const System&) = default;
		System& operator=(System&&) = default;
		virtual ~System() = default;

        [[nodiscard]] virtual bool IsActive() const = 0;
		virtual void Update(entt::registry& registry, [[maybe_unused]] float dt) = 0;
	};

}

#endif // !PROTO_SYSTEM_HPP
