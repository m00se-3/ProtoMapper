#include "ProtoMapper.hpp"

int main()
{
	proto::Mapper mapper;

	if (!mapper.Configure())
	{
		std::puts("Failed to load application configurations. Please reinstall the program.");
		return EXIT_FAILURE;
	}

	auto result = mapper.Run(); // TODO: Expand this return type to print out error messages.

	return result;
}
