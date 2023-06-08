#ifndef PROTOMAPPER_TEXTURE_HPP
#define PROTOMAPPER_TEXTURE_HPP

class ResourceManager;


/*
	This 2D Texture format is designed to be used with SDL_Surfaces.
*/
struct Texture2D
{
#ifdef USE_GLES
	using IDType = unsigned short;
#else
	using IDType = unsigned int;
#endif
	IDType ID = 0u;

	Texture2D() = default;
	Texture2D(const Texture2D&);
	explicit Texture2D(IDType id);
	~Texture2D();

	bool operator==(const Texture2D& rhs);
	bool operator==(const Texture2D& rhs) const;
	Texture2D& operator=(const Texture2D& rhs);

	Texture2D& Create();
	void Destroy();
	void Bind(unsigned int slot = 0u);
	void Bind(unsigned int slot = 0u) const;
	void Unbind();
	void Unbind() const;
	Texture2D& WriteData(const void* data, int width, int height);
	Texture2D& GenerateBlank(int w, int h, unsigned int colorValue = 0xFFFFFFFF);

	IDType Target() const;

	static void SetResourceManager(ResourceManager* ptr);


private:
	static ResourceManager* _manager;
};

#endif