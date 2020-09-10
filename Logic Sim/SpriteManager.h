#pragma once
#include "olcPixelGameEngine.h"
#include <map>
#include <filesystem>

namespace fs = std::filesystem;

class SpriteManager
{
	using SpriteMap = std::map<std::string, olc::Decal *>;
	using dir_it    = fs::directory_iterator;
	using cdir_en   = const fs::directory_entry;

private:
	SpriteMap m_sprites;

public:
	SpriteManager() noexcept;
	~SpriteManager() noexcept;

public:
	bool contains(const std::string &sFilename) const noexcept;
	void load(const std::string &sFilename) noexcept;
	
	olc::Decal *query(std::string sFilename) const noexcept;
	void load_dir(const std::string &sFilename) noexcept;
};

