#include "SpriteManager.h"

SpriteManager::SpriteManager() noexcept
{
	m_sprites = SpriteMap();
}

SpriteManager::~SpriteManager() noexcept
{
	for (std::pair<std::string, olc::Decal *> vEntry : m_sprites)
		delete vEntry.second;
}

bool SpriteManager::contains(const std::string &sFilename) const noexcept
{
	return m_sprites.contains(sFilename);
}

void SpriteManager::load(const std::string &sFilename) noexcept
{
	auto StringEndsWith = [](const std::string &str, const std::string &end)
	{
		size_t diff = str.size() - end.size();
		for (size_t i = diff; i < str.size(); i++)
			if (str.at(i) != end.at(i - diff))
				return false;
		return true;
	};

	if (!contains(sFilename))
	{
		if (StringEndsWith(sFilename, ".png"))
		{
			m_sprites.insert(std::make_pair(sFilename, new olc::Decal(new olc::Sprite(sFilename))));
		}
		else if (StringEndsWith(sFilename, ".pgepng"))
		{
			olc::Sprite *spr = new olc::Sprite();
			spr->LoadFromPGESprFile(sFilename);
			m_sprites.insert(std::make_pair(sFilename, new olc::Decal(spr)));
		}
	}
}

olc::Decal *SpriteManager::query(std::string sFilename) const noexcept
{
	auto ReplInString = [](std::string &str)
	{
		for (size_t i = 0; i < str.size(); i++)
			if (str.at(i) == '/')
				str.at(i) = '\\';
	};

	ReplInString(sFilename);
	return m_sprites.at(sFilename);
}

void SpriteManager::load_dir(const std::string &sFilename) noexcept
{
	for (cdir_en &entry : dir_it(sFilename))
		load(entry.path().string());
}
