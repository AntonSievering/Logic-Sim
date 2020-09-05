#pragma once
#include "ICX.h"
#include "SpriteManager.h"

// Define USE_PYTHON_PNG_SAVING to enable the saving of png files using the Python Imaging Library
// Requires the PIL, otherwise it will fail
// it features a better data compression than the standard pge save method

class Circuit : public IC
{
public:
	std::string sPath;
	std::vector<IC*> vICs;
	std::vector<IC*> vInputGates;
	std::vector<IC*> vOutputGates;
	std::thread tSave;

private:
	void LoadFromFile(std::string sFilename, olc::vi2d pos, SpriteManager* sm);
	void GenerateSprite(std::string sPath);
	void GenerateSprite_Thread(std::string sPath);
	void SaveToFile();

public:
	Circuit();
	Circuit(std::string sFilename, olc::vi2d pos, SpriteManager* sm);
	~Circuit();

	virtual void Update() override;

	virtual void save();
};
