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
		virtual ~System() = default;

	protected:

		virtual bool IsActive() const = 0;
		virtual void Update(entt::registry& registry, [[maybe_unused]] float dt) = 0;
	};

}

#endif // !PROTO_SYSTEM_HPP
