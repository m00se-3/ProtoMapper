#ifndef PROTOMAPPER_UICONTAINER_HPP
#define PROTOMAPPER_UICONTAINER_HPP

#include "Texture.hpp"
#include "SimpleIni.h"

#include <mutex>
#include <map>
#include <string>
#include <filesystem>

// Forward declarations.
class Renderer;



class UIContainer
{
	// For thread-safe access.
	std::mutex _mutex;

	/*
		This holds the data that is customizable for the builtin uier interface.
	*/
	std::string _dataFile;
	CSimpleIniA _uiData;
	bool _updateUIData = false;

	Texture2D _fontTexture;


public:
	UIContainer();
	~UIContainer();

	bool SetData(const std::filesystem::path& filepath);
	void UpdateUI(float wWidth, float wHeight); // This is the big function.
	void CompileUI();
	void DrawUI(Renderer* ren);

	void Lock();
	void Unlock();


	static void SetResourceManager(Texture2DManager* ptr);


private:
	static Texture2DManager* _texMan;
	

};

#endif // !PROTOMAPPER_UICONTAINER_HPP
