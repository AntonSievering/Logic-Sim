#pragma once
#include "PGEGui.h"
#include "SpriteManager.h"


class IC;

struct Node
{
	IC* from; int posFrom;
	IC* to;   int posTo;
	bool bCharge;
	olc::Decal *pBlack;
	olc::Decal *pGreen;

public:
	Node();
	Node(IC* pFrom, IC* pTo, int posFrom, int posTo);
	~Node();

public:
	inline virtual void Draw(PGEGui* pge);
};


class IC
{
public:
	std::string sName;
	std::string sIdentifier;
	olc::Decal* decIC;
	olc::Decal* decNode;
	olc::Decal* decNodeSelected;
	olc::Decal* decNodeHovered;
	olc::Decal *decNodeFilled;
	olc::vf2d pos;
	std::vector<std::vector<Node*>> vOutNodes; int nOutNodes;
	std::vector<Node*> vInNodes;               int nInNodes;
	std::vector<olc::vf2d> vOutNodesToDraw;
	std::vector<olc::vf2d> vInNodesToDraw;
	std::vector<std::string> vOutNames;
	std::vector<std::string> vInNames;

protected:
	void prepare(olc::vi2d pos);

public:
	IC();
	IC(olc::Decal* decIC,     olc::vi2d pos, SpriteManager *sm);
	IC(std::string sFilename, olc::vi2d pos, SpriteManager *sm);
	virtual ~IC();
	
public:
	void addOutNode(int nNodeIdx, Node* node);
	void addInNode (int nNodeIdx, Node* node);
	void deleteOutNode(int nNodeIdx, Node* node);
	void deleteInNode (int nNodeIdx);
	
	int getSelectedOutNode(PGEGui* pge); // -1 means no one is selected
	int getSelectedInNode (PGEGui* pge);  // -1 means no one is selected
	bool getInNodeState(size_t nInNode); // 1 means has a connection, 0 the opposite
	std::vector<Node*>* getInNodes();
	std::vector<std::vector<Node*>>* getOutNodes();

	olc::vf2d getOutNodePos(int nNodeIdx);
	olc::vf2d getInNodePos (int nNodeIdx);

	void DrawNodes(PGEGui* pge, int nSelected);
	void DrawNames(PGEGui* pge);
	virtual inline void Draw(PGEGui* pge, int nSelected);
	std::vector<bool>   getInputs();
	
	void load_sprites(const std::string &sFilename, SpriteManager* sm);

	inline virtual void Update();

	bool collides_with(PGEGui* pge, olc::vf2d pos);
	virtual void pressed();

	void UpdatePos(olc::vf2d vNewPos);
	olc::vf2d GetPos();

	virtual bool is_input();
	virtual bool is_output();
};