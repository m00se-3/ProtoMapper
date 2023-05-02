#ifndef MAPPER_MAP_HPP
#define MAPPER_MAP_HPP

#include <memory>

class ProtoMapper;

class Map
{
protected:
	ProtoMapper* _parent = nullptr;

public:
	Map() = default;
	virtual ~Map();

	virtual bool Create(ProtoMapper* par, unsigned int, unsigned int) = 0;	// The size of the map in cell width and height. 
	virtual bool Generate(size_t) = 0;

};

#endif