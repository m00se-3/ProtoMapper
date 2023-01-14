#ifndef PROTOMAPPER_MAIN_HPP
#define PROTOMAPPER_MAIN_HPP

#include "SFML/Window.hpp"
#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"
#include "fmt/format.h"

#include <string>

class ProtoMapper
{
	bool _appRunning = true;

public:
	ProtoMapper() = default;
	~ProtoMapper() = default;

	void Run();
};

#endif