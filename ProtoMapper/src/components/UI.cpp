#include "components/UI.hpp"

UIStructure::UIStructure(UIStructure&& other)
	: entities(std::move(other.entities))
{
}
