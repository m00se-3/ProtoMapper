#ifndef MAPPER_MAP_HPP
#define MAPPER_MAP_HPP

#include <memory>

class ProtoMapper;

class Map
{
protected:

public:
	Map() = default;
	virtual ~Map();

	virtual bool Create(unsigned int, unsigned int) = 0;	// The size of the map in cell width and height. 
	virtual bool Generate(size_t) = 0;

	template <typename MapType>
	static MapType* Get(const std::shared_ptr<Map>& ptr) { return (MapType*)ptr.get(); }

};

#endif