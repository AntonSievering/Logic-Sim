#include "ICX.h"

// AND GATE
AndGate::AndGate()
{
}

AndGate::AndGate(olc::vi2d pos, SpriteManager* sm)
{
	this->pos = pos;
	sName = "And Gate";
	sIdentifier = "AND";

	load_sprites("sprites/and.png", sm);
	prepare(pos);
}

inline void AndGate::Update()
{
	std::vector<bool> vInputs = getInputs();

	bool b0 = vInputs.at(0);
	bool b1 = vInputs.at(1);
	bool bValue = b0 && b1;

	for (Node* node : vOutNodes.at(0))
		if (node != nullptr)
			node->bCharge = bValue;
}


// NAND GATE
NandGate::NandGate()
{
}

NandGate::NandGate(olc::vi2d pos, SpriteManager *sm)
{
	this->pos = pos;
	sName = "Nand Gate";
	sIdentifier = "NAND";

	load_sprites("sprites/nand.png", sm);
	prepare(pos);
}

inline void NandGate::Update()
{
	std::vector<bool> vInputs = getInputs();

	bool b0 = vInputs.at(0);
	bool b1 = vInputs.at(1);
	bool bValue = !(b0 && b1);

	for (Node* node : vOutNodes.at(0))
	{
		if (node != nullptr)
			node->bCharge = bValue;
	}
}


// OR GATE
OrGate::OrGate()
{
}

OrGate::OrGate(olc::vi2d pos, SpriteManager *sm)
{
	this->pos = pos;
	sName = "Or Gate";
	sIdentifier = "OR";

	load_sprites("sprites/or.png", sm);
	prepare(pos);
}

inline void OrGate::Update()
{
	std::vector<bool> vInputs = getInputs();

	bool b0 = vInputs.at(0);
	bool b1 = vInputs.at(1);
	bool bValue = b0 || b1;

	for (Node* node : vOutNodes.at(0))
		if (node != nullptr)
			node->bCharge = bValue;
}


// NOR GATE
NorGate::NorGate()
{
}

NorGate::NorGate(olc::vi2d pos, SpriteManager* sm)
{
	this->pos = pos;
	sName = "Nor Gate";
	sIdentifier = "NOR";

	load_sprites("sprites/nor.png", sm);
	prepare(pos);
}

inline void NorGate::Update()
{
	std::vector<bool> vInputs = getInputs();

	bool b0 = vInputs.at(0);
	bool b1 = vInputs.at(1);
	bool bValue = !(b0 || b1);

	for (Node* node : vOutNodes.at(0))
		if (node != nullptr)
			node->bCharge = bValue;
}


// XOR GATE
XorGate::XorGate()
{
}

XorGate::XorGate(olc::vi2d pos, SpriteManager* sm)
{
	this->pos = pos;
	sName = "Xor Gate";
	sIdentifier = "XOR";

	load_sprites("sprites/xor.png", sm);
	prepare(pos);
}

inline void XorGate::Update()
{
	std::vector<bool> vInputs = getInputs();

	bool b0 = vInputs.at(0);
	bool b1 = vInputs.at(1);
	bool bValue = b0 != b1;

	for (Node* node : vOutNodes.at(0))
		if (node != nullptr)
			node->bCharge = bValue;
}


// XNOR GATE
XnorGate::XnorGate()
{
}

XnorGate::XnorGate(olc::vi2d pos, SpriteManager* sm)
{
	this->pos = pos;
	sName = "Xnor Gate";
	sIdentifier = "XNOR";
	
	load_sprites("sprites/xnor.png", sm);
	prepare(pos);
}

inline void XnorGate::Update()
{
	std::vector<bool> vInputs = getInputs();

	bool b0 = vInputs.at(0);
	bool b1 = vInputs.at(1);
	bool bValue = b0 == b1;
	
	for (Node* node : vOutNodes.at(0))
		if (node != nullptr)
			node->bCharge = bValue;
}

// BUFFER GATE
BufferGate::BufferGate()
{
}

BufferGate::BufferGate(olc::vi2d pos, SpriteManager* sm)
{
	this->pos = pos;
	sName = "Buffer Gate";
	sIdentifier = "BUF";

	load_sprites("sprites/buf.png", sm);
	prepare(pos);
}

inline void BufferGate::Update()
{
	Node* outnode = this->vOutNodes.at(0).at(0);
	Node* innode = this->vInNodes.at(0);

	if (outnode != nullptr)
	{
		if (innode != nullptr)
			outnode->bCharge = innode->bCharge;
		else
			outnode->bCharge = false;
	}
}

// NOT GATE
NotGate::NotGate()
{
}

NotGate::NotGate(olc::vi2d pos, SpriteManager* sm)
{
	this->pos = pos;
	sName = "Not Gate";
	sIdentifier = "NOT";

	load_sprites("sprites/not.png", sm);
	prepare(pos);
}

inline void NotGate::Update()
{
	Node* outnode = vOutNodes.at(0).at(0);
	Node* innode = vInNodes.at(0);

	if (outnode != nullptr)
	{
		if (innode != nullptr)
			outnode->bCharge = !innode->bCharge;
		else
			outnode->bCharge = true;
	}
}

ButtonGate::ButtonGate()
{
	bPressed = false;
	decPressed = nullptr;
}

ButtonGate::ButtonGate(olc::vi2d pos, SpriteManager* sm)
{
	this->pos = pos;
	sName = "Toggle Button";
	sIdentifier = "BTN";
	
	load_sprites("sprites/btn0.png", sm);
	decPressed = sm->query("sprites/btn1.png");
	prepare(pos);
}

inline void ButtonGate::Draw(PGEGui* pge, int nSelected)
{
	if (bPressed) pge->DrawDecal(pge->WorldToScreen(pos), decPressed, { pge->GetZoom(), pge->GetZoom() });
	else pge->DrawDecal(pge->WorldToScreen(pos), decIC, { pge->GetZoom(), pge->GetZoom() });
	DrawNodes(pge, nSelected);
	DrawNames(pge);
}

inline void ButtonGate::Update()
{
	for (std::vector<Node*>& vOutNodesLayer : vOutNodes)
		for (Node* node : vOutNodesLayer)
			if (node != nullptr)
				node->bCharge = bPressed;
}

inline void ButtonGate::pressed()
{
	bPressed = !bPressed;
}

inline bool ButtonGate::is_input()
{
	return true;
}


// The Light Bulb
LightBulbGate::LightBulbGate()
{
	decLit = nullptr;
	bCharge = false;
}

LightBulbGate::LightBulbGate(olc::vi2d pos, SpriteManager* sm)
{
	this->pos = pos;
	sName = "Light Bulb";
	sIdentifier = "BULB";

	load_sprites("sprites/bulb0.png", sm);
	decLit = sm->query("sprites/bulb1.png");
	prepare(pos);
}

inline void LightBulbGate::Draw(PGEGui* pge, int nSelected)
{
	if (bCharge) pge->DrawDecal(pge->WorldToScreen(pos), decLit, { pge->GetZoom(), pge->GetZoom() });
	else pge->DrawDecal(pge->WorldToScreen(pos), decIC, { pge->GetZoom(), pge->GetZoom() });
	DrawNodes(pge, nSelected);
	DrawNames(pge);
}

inline void LightBulbGate::Update()
{
	Node* node = vInNodes.at(0);
	if (node != nullptr) bCharge = node->bCharge;
	else bCharge = false;
}

inline bool LightBulbGate::is_output()
{
	return true;
}


// The 7-Segment-Display
SevenSegmentDisplay::SevenSegmentDisplay()
{
	decHorizontal = decVertical = nullptr;
	pSegments = std::array<bool, 7>();
}

SevenSegmentDisplay::SevenSegmentDisplay(olc::vi2d pos, SpriteManager* sm)
{
	pSegments = std::array<bool, 7>();
	
	this->pos = pos;
	sName = "7 Segment Display";
	sIdentifier = "7SEG";

	load_sprites("sprites/7segment.png", sm);
	decHorizontal = sm->query("sprites/hsegment.png");
	decVertical = sm->query("sprites/vsegment.png");
	prepare(pos);

	vInNames = { "A", "B", "C", "D", "E", "F", "G" };
}

inline void SevenSegmentDisplay::Draw(PGEGui* pge, int nSelected)
{
	olc::vf2d vZoom = { pge->GetZoom(), pge->GetZoom() };
	// Draw the base sprite
	pge->DrawDecal(pge->WorldToScreen(pos), decIC, vZoom);

	// Draw the 7 segments
	if (pSegments.at(0)) pge->DrawDecal(pge->WorldToScreen(pos + olc::vf2d(32, 22)), decHorizontal, vZoom);
	if (pSegments.at(1)) pge->DrawDecal(pge->WorldToScreen(pos + olc::vf2d(94, 28)), decVertical, vZoom);
	if (pSegments.at(2)) pge->DrawDecal(pge->WorldToScreen(pos + olc::vf2d(94, 112)), decVertical, vZoom);
	if (pSegments.at(3)) pge->DrawDecal(pge->WorldToScreen(pos + olc::vf2d(32, 190)), decHorizontal, vZoom);
	if (pSegments.at(4)) pge->DrawDecal(pge->WorldToScreen(pos + olc::vf2d(26, 112)), decVertical, vZoom);
	if (pSegments.at(5)) pge->DrawDecal(pge->WorldToScreen(pos + olc::vf2d(26, 28)), decVertical, vZoom);
	if (pSegments.at(6)) pge->DrawDecal(pge->WorldToScreen(pos + olc::vf2d(32, 106)), decHorizontal, vZoom);

	DrawNodes(pge, nSelected);
	DrawNames(pge);
}

inline void SevenSegmentDisplay::Update()
{
	std::vector<bool> vInputs = getInputs();
	for (int i = 0; i < 7; i++)
		pSegments.at(i) = vInputs.at(i);
}


// The Seven Segment Display Encoder
SevenSegmentEncoder::SevenSegmentEncoder()
{
	GenerateStates();
}

SevenSegmentEncoder::SevenSegmentEncoder(olc::vi2d pos, SpriteManager* sm)
{
	this->pos = pos;
	sName = "7 Segment Display Encoder";
	sIdentifier = "SDE";

	load_sprites("sprites/encoder7s.png", sm);
	prepare(pos);
	GenerateStates();
	
	vInNames = { "0", "1", "2", "3" };
	vOutNames = { "A", "B", "C", "D", "E", "F", "G" };
}

void SevenSegmentEncoder::GenerateStates()
{
	pStates[0x0] = 0b00111111;
	pStates[0x1] = 0b00000110;
	pStates[0x2] = 0b01011011;
	pStates[0x3] = 0b01001111;
	pStates[0x4] = 0b01100110;
	pStates[0x5] = 0b01101101;
	pStates[0x6] = 0b01111101;
	pStates[0x7] = 0b00000111;
	pStates[0x8] = 0b01111111;
	pStates[0x9] = 0b01101111;
	pStates[0xA] = 0b01110111;
	pStates[0xB] = 0b01111100;
	pStates[0xC] = 0b01011000;
	pStates[0xD] = 0b01011110;
	pStates[0xE] = 0b01111001;
	pStates[0xF] = 0b01110001;
}

inline void SevenSegmentEncoder::Update()
{
	std::vector<bool> vInputs = getInputs();

	int8_t addr = 0;
	int mul = 8;

	for (int i = 0; i < 4; i++)
	{
		addr += vInputs.at(i) * mul;
		mul >>= 1;
	}

	int8_t result = pStates[addr];

	for (int i = 0; i < 7; i++)
	{
		bool bValue = ((result >> i) & 1) == 1;

		for (Node* node : vOutNodes.at(i))
			if (node != nullptr)
				node->bCharge = bValue;
	}
}

// The frequency generator
ClockGate::ClockGate()
{
	fFrequency = 0.0f;
	fPeriod = 0.0f;
	tStart = std::chrono::system_clock::now();
}

ClockGate::ClockGate(olc::vi2d pos, SpriteManager* sm)
{
	fFrequency = 5.0f;

	this->pos = pos;
	sName = "Clock 5Hz";
	sIdentifier = "clk";
	tStart = std::chrono::system_clock::now();

	load_sprites("sprites/clock.png", sm);
	prepare(pos);
}

inline void ClockGate::Update()
{
	std::string sFiltered;
	for (char c : sName)
		if (c == 46 || (c > 47 && c < 58))
			sFiltered.push_back(c);

	fFrequency = atof(sFiltered.c_str());
	fPeriod = 1.0f / fFrequency;

	float fTimeSinceLastCheck = (float)std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - tStart).count() / 1e9f;

	if (fTimeSinceLastCheck < 0.5f * fPeriod)
	{
		for (Node *node : vOutNodes.at(0))
			if (node != nullptr)
				node->bCharge = true;
	}
	else
	{
		for (Node *node : vOutNodes.at(0))
			if (node != nullptr)
				node->bCharge = false;
	}
		

	if (fTimeSinceLastCheck > fPeriod)
		tStart = std::chrono::system_clock::now();
}
