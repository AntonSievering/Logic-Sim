#pragma once
#include "IC.h"

// The AND Gate
class AndGate : public IC
{
public:
	AndGate();
	AndGate(olc::vi2d pos, SpriteManager *sm);

public:
	inline virtual void Update() override;
};

// The NAND Gate
class NandGate : public IC
{
public:
	NandGate();
	NandGate(olc::vi2d pos, SpriteManager *sm);

public:
	inline virtual void Update() override;
};

// The OR Gate
class OrGate : public IC
{
public:
	OrGate();
	OrGate(olc::vi2d pos, SpriteManager *sm);

public:
	inline virtual void Update() override;
};

// The NOR Gate
class NorGate : public IC
{
public:
	NorGate();
	NorGate(olc::vi2d pos, SpriteManager *sm);

public:
	inline virtual void Update() override;
};

// The XOR Gate
class XorGate : public IC
{
public:
	XorGate();
	XorGate(olc::vi2d pos, SpriteManager *sm);

public:
	inline virtual void Update() override;
};

// The XNOR Gate
class XnorGate : public IC
{
public:
	XnorGate();
	XnorGate(olc::vi2d pos, SpriteManager *sm);

public:
	inline virtual void Update() override;
};

// The BUFFER Gate - does nothing - input = output
class BufferGate : public IC
{
public:
	BufferGate();
	BufferGate(olc::vi2d pos, SpriteManager *sm);

public:
	inline virtual void Update() override;
};

// The NOT Gate
class NotGate : public IC
{
public:
	NotGate();
	NotGate(olc::vi2d pos, SpriteManager *sm);

public:
	inline virtual void Update() override;
};

// The Button
class ButtonGate : public IC
{
public:
	bool bPressed;
	olc::Decal* decPressed;

public:
	ButtonGate();
	ButtonGate(olc::vi2d pos, SpriteManager *sm);

public:
	inline virtual void Draw(PGEGui* pge, int nSelected) override;
	inline virtual void Update() override;
	inline void pressed() override;
	inline bool is_input() override;
};

// The Light Bulb
class LightBulbGate : public IC
{
public:
	olc::Decal* decLit;
	bool bCharge;

public:
	LightBulbGate();
	LightBulbGate(olc::vi2d pos, SpriteManager *sm);

public:
	inline virtual void Draw(PGEGui* pge, int nSelected) override;
	inline virtual void Update() override;
	inline bool is_output() override;
};

// The Seven Segment Display
class SevenSegmentDisplay : public IC
{
public:
	olc::Decal* decVertical;
	olc::Decal* decHorizontal;
	std::array<bool, 7> pSegments;

public:
	SevenSegmentDisplay();
	SevenSegmentDisplay(olc::vi2d pos, SpriteManager *sm);

public:
	inline virtual void Draw(PGEGui* pge, int nSelected) override;
	inline virtual void Update() override;
};

class SevenSegmentEncoder : public IC
{
public:
	std::array<int8_t, 16> pStates;

public:
	SevenSegmentEncoder();
	SevenSegmentEncoder(olc::vi2d pos, SpriteManager *sm);

private:
	void GenerateStates();

public:
	inline virtual void Update() override;
};

class ClockGate : public IC
{
private:
	float fFrequency;
	float fPeriod;
	std::chrono::time_point<std::chrono::system_clock> tStart;

public:
	ClockGate();
	ClockGate(olc::vi2d pos, SpriteManager *sm);

public:
	inline virtual void Update() override;
};