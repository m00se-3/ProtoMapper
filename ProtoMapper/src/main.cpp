#include <cstdio>
#include <cstdlib>

import proto.Mapper;

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
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