#include "IC.h"

Node::Node()
{
	from = to = nullptr;
	bCharge = false;
	posFrom = posTo = 0;
	olc::Sprite *spr = new olc::Sprite(1, 1); spr->SetPixel(0, 0, olc::BLACK);
	pBlack = new olc::Decal(spr);
	spr->SetPixel(0, 0, olc::GREEN);
	pGreen = new olc::Decal(spr);
}

Node::Node(IC* from, IC* to, int posFrom, int posTo)
{
	this->from = from;
	this->to = to;
	bCharge = false;
	olc::Sprite *spr = new olc::Sprite(1, 1); spr->SetPixel(0, 0, olc::BLACK);
	pBlack = new olc::Decal(spr);
	spr->SetPixel(0, 0, olc::GREEN);
	pGreen = new olc::Decal(spr);

	this->posFrom = posFrom;
	this->posTo = posTo;
}

Node::~Node()
{
	if (to != nullptr)
		to->deleteInNode(posTo);
	if (from != nullptr)
		from->deleteOutNode(posFrom, this);

	delete pBlack;
	delete pGreen;
}

void Node::Draw(PGEGui* pge)
{
	float fLength = 4.0f * pge->GetZoom();

	olc::vf2d posFromNewWorld = from->getOutNodePos(posFrom) + olc::vf2d(4, 4);
	olc::vf2d posToNewWorld = to->getInNodePos(posTo) + olc::vf2d(3, 4);
	
	olc::vf2d posFromNew = pge->WorldToScreen(posFromNewWorld);
	olc::vf2d posToNew = pge->WorldToScreen(posToNewWorld);

	posFromNew.x -= 2.0f;
	posToNew.x += 2.0f;

	auto _2dCollision = [&](olc::vi2d pos0, olc::vi2d pos1, olc::vi2d pos2, olc::vi2d pos3)
	{
		float s1_x, s1_y, s2_x, s2_y;
		s1_x = pos1.x - pos0.x;     s1_y = pos1.y - pos0.y;
		s2_x = pos3.x - pos2.x;     s2_y = pos3.y - pos1.y;

		float s, t;
		s = (-s1_y * (pos0.x - pos2.x) + s1_x * (pos0.y - pos2.y)) / (-s2_x * s1_y + s1_x * s2_y);
		t = (s2_x * (pos0.y - pos2.y) - s2_y * (pos0.x - pos2.x)) / (-s2_x * s1_y + s1_x * s2_y);

		if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
		{
			return 1;
		}

		return 0; // No collision
	};

	// Draw if visible
	if (posFromNew.x < 0 && posToNew.x   > 800 && posFromNew.y < 0 && posToNew.y > 800     ||
		posFromNew.x > 0 && posFromNew.x < 800 && posFromNew.y > 0 && posFromNew.y      < 800 ||
		posToNew.x   > 0 && posToNew.x   < 800 && posToNew.y   > -fLength && posToNew.y < 800  ||
		!_2dCollision({ 0, 0 }, { 0, 800 }, posFromNew, posToNew)     ||
		!_2dCollision({ 0, 800 }, { 800, 800 }, posFromNew, posToNew) ||
		!_2dCollision({ 800, 800 }, { 800, 0 }, posFromNew, posToNew) ||
		!_2dCollision({ 800, 0 }, { 0, 0 }, posFromNew, posToNew))
	{
		// calculate x and y offset from the draw point
		float fSlope = -(posToNew.x - posFromNew.x) / (posToNew.y - posFromNew.y);
		float fAngle = atanf(fSlope);
		float dx = 0.5f * fLength * cosf(fAngle);
		float dy = 0.5f * fLength * sinf(fAngle);

		// calculate the four points
		olc::vf2d pos0 = posFromNew + olc::vf2d(dx, dy);
		olc::vf2d pos1 = posFromNew - olc::vf2d(dx, dy);
		olc::vf2d pos2 = posToNew - olc::vf2d(dx, dy);
		olc::vf2d pos3 = posToNew + olc::vf2d(dx, dy);

		if (bCharge)
			pge->DrawWarpedDecal(pGreen, { pos0, pos1, pos2, pos3 });
		else
			pge->DrawWarpedDecal(pBlack, { pos0, pos1, pos2, pos3 });
	}
}



void IC::prepare(olc::vi2d pos)
{

	// Input nodes
	size_t nStreak = 0;
	for (int i = 0; i < decIC->sprite->height; i++)
	{
		olc::Pixel col = decIC->sprite->GetPixel(0, i);
		if (col == olc::BLACK) nStreak++;
		else nStreak = 0;

		if (nStreak == 4)
		{
			nStreak = 0;
			vInNodesToDraw.push_back(olc::vi2d(pos.x - 4, pos.y - 5 + i));
			nInNodes++;
			vInNames.push_back("");
		}
	}

	// output nodes
	nStreak = 0;
	for (int i = 0; i < decIC->sprite->height; i++)
	{
		olc::Pixel col = decIC->sprite->GetPixel(decIC->sprite->width - 1, i);
		if (col == olc::BLACK) nStreak++;
		else
		{
			if (nStreak == 4)
			{
				nStreak = 0;
				vOutNodesToDraw.push_back(olc::vi2d(pos.x + decIC->sprite->width - 1, pos.y - 6 + i));
				nOutNodes++;
				vOutNames.push_back("");
			}
			nStreak = 0;
		}
	}

	// create the std::vector for the out nodes
	for (int i = 0; i < nOutNodes; i++)
		vOutNodes.push_back(std::vector<Node*>());
	for (std::vector<Node*>& vNodes : vOutNodes)
		vNodes.push_back(nullptr);
	// create the std::vector for the in nodes
	vInNodes = std::vector<Node*>();
	for (int i = 0; i < nInNodes; i++)
	{
		vInNodes.push_back(nullptr);
	}
}

IC::IC()
{
	decIC = nullptr;
	nOutNodes = nInNodes = 0;
	sName = "IC";
	sIdentifier = "IC";
}

IC::IC(olc::Decal* decIC, olc::vi2d pos, SpriteManager *sm)
{
	this->decIC = decIC;
	this->pos = pos;
	nOutNodes = nInNodes = 0;
	sName = "IC";
	sIdentifier = "IC";

	load_sprites("", sm);
	prepare(pos);
}

IC::IC(std::string sFilename, olc::vi2d pos, SpriteManager *sm)
{
	this->pos = pos;
	nOutNodes = nInNodes = 0;
	sName = "IC";

	load_sprites("", sm);
	prepare(pos);
}

IC::~IC()
{
}

void IC::deleteInNode(int nNodeIdx)
{
	if (nNodeIdx > -1 && nNodeIdx < vInNodes.size())
		vInNodes.at(nNodeIdx) = nullptr;
}

void IC::deleteOutNode(int nNodeIdx, Node* node)
{
	std::vector<Node *> &vNodes = vOutNodes.at(nNodeIdx);
	for (int i = 0; i < vNodes.size(); i++)
	{
		if (vNodes.at(i) == node)
		{
			if (vNodes.size() > 1)
				vNodes.erase(vNodes.begin() + i);
			else
				vNodes.at(i) = nullptr;
		}
	}
}

void IC::addOutNode(int nNodeIdx, Node* node)
{
	// if the first one is a nullptr, replace it
	if (vOutNodes.at(nNodeIdx).at(0) == nullptr) vOutNodes.at(nNodeIdx).at(0) = node;
	else vOutNodes.at(nNodeIdx).push_back(node);
}

void IC::addInNode(int nNodeIdx, Node* node)
{
	delete vInNodes.at(nNodeIdx);
	vInNodes.at(nNodeIdx) = node;
}

int IC::getSelectedOutNode(PGEGui* pge)
{
	olc::vf2d vMouse = pge->ScreenToWorld(olc::vf2d(pge->GetMouseX(), pge->GetMouseY()));
	
	int i = 0;
	for (olc::vf2d& pos : vOutNodesToDraw)
	{
		if (vMouse.x > pos.x && vMouse.y > pos.y && vMouse.x < pos.x + 8 && vMouse.y < pos.y + 8)
			return i;
		i++;
	}

	return -1;
}

int IC::getSelectedInNode(PGEGui* pge)
{
	olc::vf2d vMouse = pge->ScreenToWorld(olc::vf2d(pge->GetMouseX(), pge->GetMouseY()));
	
	int i = 0;
	for (olc::vf2d& pos : vInNodesToDraw)
	{
		if (vMouse.x > pos.x && vMouse.y > pos.y && vMouse.x < pos.x + 8 && vMouse.y < pos.y + 8)
			return i;
		i++;
	}

	return -1;
}

bool IC::getInNodeState(size_t nInNode)
{
	return vInNodes.at(nInNode) != nullptr;
}

std::vector<Node*>* IC::getInNodes()
{
	return &vInNodes;
}

std::vector<std::vector<Node*>>* IC::getOutNodes()
{
	return &vOutNodes;
}

olc::vf2d IC::getOutNodePos(int nNodeIdx)
{
	return vOutNodesToDraw.at(nNodeIdx);
}

olc::vf2d IC::getInNodePos(int nNodeIdx)
{
	return vInNodesToDraw.at(nNodeIdx);
}

void IC::DrawNodes(PGEGui* pge, int nSelected)
{
	// draw the lines to other gates
	for (std::vector<Node*> vNodes : vOutNodes)
	{
		for (Node* node : vNodes)
		{
			if (node != nullptr)
				node->Draw(pge);
		}
	}

	// Draw the out nodes
	int i = 0;
	for (olc::vf2d& pos : vOutNodesToDraw)
	{
		// if the node is visible and not "filled", draw it
		olc::vf2d screenpos = pge->WorldToScreen(pos);
		if (screenpos.x > -4.0f * pge->GetZoom() && screenpos.y > -4.0f * pge->GetZoom() && screenpos.x <= 800 + 4.0f * pge->GetZoom() && screenpos.y <= 800 + 4.0f * pge->GetZoom())
		{
			// Highlights the clicked node
			if (nSelected == i)
			{
				pge->DrawDecal(screenpos, decNodeSelected, olc::vf2d(pge->GetZoom(), pge->GetZoom()) / 8.0f);
			}
			else if (getSelectedOutNode(pge) == i)
			{
				pge->DrawDecal(screenpos, decNodeHovered, olc::vf2d(pge->GetZoom(), pge->GetZoom()) / 8.0f);
			}
			// draws the basic node
			else if (vOutNodes.at(i).at(0) == nullptr)
			{
				pge->DrawDecal(screenpos, decNode, olc::vf2d(pge->GetZoom(), pge->GetZoom()) / 8.0f);
			}
			else
			{
				pge->DrawDecal(screenpos, decNodeFilled, olc::vf2d(pge->GetZoom(), pge->GetZoom()) / 8.0f);
			}
		}
		i++;
	}

	// draw the in nodes
	i = 0;
	for (olc::vf2d& pos : vInNodesToDraw)
	{
		// if the node is visible and not "filled", draw it
		olc::vf2d screenpos = pge->WorldToScreen(pos);
		if (screenpos.x > -4.0f * pge->GetZoom() && screenpos.y > -4.0f * pge->GetZoom() && screenpos.x <= 800 + 4.0f * pge->GetZoom() && screenpos.y <= 800 + 4.0f * pge->GetZoom())
		{
			if (getSelectedInNode(pge) == i)
			{
				pge->DrawDecal(screenpos, decNodeHovered, olc::vf2d(pge->GetZoom(), pge->GetZoom()) / 8.0f);
			}
			else if (vInNodes.at(i) == nullptr)
			{
				pge->DrawDecal(screenpos, decNode, olc::vf2d(pge->GetZoom(), pge->GetZoom()) / 8.0f);
			}
			else
			{
				pge->DrawDecal(screenpos, decNodeFilled, olc::vf2d(pge->GetZoom(), pge->GetZoom()) / 8.0f);
			}
		}
		i++;
	}
}

void IC::DrawNames(PGEGui* pge)
{
	if (pge->GetZoom() > 1.0f)
	{
		int i = 0;
		const olc::vf2d vZoom = { 0.5f * pge->GetZoom(), 0.5f * pge->GetZoom() };

		// Input Nodes Names
		for (std::string &str : vInNames)
		{
			olc::vf2d pos = pge->WorldToScreen(getInNodePos(i) + olc::vf2d(10.0f, 0.0f));
			pge->DrawStringDecal(pos, str, olc::WHITE, vZoom);
			i++;
		}

		i = 0;
		// Output Nodes Names
		for (std::string &str : vOutNames)
		{
			olc::vf2d pos = pge->WorldToScreen(getOutNodePos(i) - olc::vf2d(10.0f, 0.0f));

			pge->DrawStringDecal(pos, str, olc::WHITE, vZoom);

			i++;
		}

		// Draw the name
		int namelen = 4.0f * sName.size();
		olc::vi2d vTextPos = pge->WorldToScreen(olc::vf2d(pos.x + 0.5f * decIC->sprite->width - 0.5f * namelen, pos.y + decIC->sprite->height));
		olc::vi2d vSize = olc::vf2d(pge->GetZoom(), pge->GetZoom()) * olc::vf2d(namelen, 8.0f);
		if (vTextPos.x < 800 && vTextPos.y < 800 && vTextPos.x + vSize.x > 0 && vTextPos.y + vSize.y > 0)
			pge->DrawStringDecal(vTextPos, sName, olc::WHITE, vZoom);
	}
}

void IC::Draw(PGEGui* pge, int nSelected)
{
	// Draw the IC its self
	olc::vi2d vPos = pge->WorldToScreen(pos);
	olc::vi2d vSize = olc::vf2d(pge->GetZoom(), pge->GetZoom()) * olc::vi2d(decIC->sprite->width, decIC->sprite->height);
	if (vPos.x < 800 && vPos.y < 800 && vPos.x + vSize.x > 0 && vPos.y + vSize.y > 0)
		pge->DrawDecal(vPos, decIC, { pge->GetZoom(), pge->GetZoom() });

	DrawNodes(pge, nSelected);
	DrawNames(pge);
}

std::vector<bool> IC::getInputs()
{
	std::vector<bool> vInputs = std::vector<bool>();
	
	for (Node* node : vInNodes)
	{
		if (node != nullptr)
			vInputs.push_back(node->bCharge);
		else
			vInputs.push_back(false);
	}

	return vInputs;
}

void IC::load_sprites(const std::string& sFilename, SpriteManager* sm)
{
	if (sFilename != "")
		decIC       = sm->query(sFilename);
	else
		decIC       = nullptr;

	decNode         = sm->query("sprites/node.png");
	decNodeSelected = sm->query("sprites/node_selected.png");
	decNodeHovered  = sm->query("sprites/node_hovered.png");
	decNodeFilled   = sm->query("sprites/node_filled.png");
}

inline void IC::Update()
{
}

bool IC::collides_with(PGEGui* pge, olc::vf2d vMouse)
{
	olc::vf2d vMouseWorld(pge->ScreenToWorld(vMouse));
	return (pos.x < vMouseWorld.x && pos.x + decIC->sprite->width  > vMouseWorld.x &&
			pos.y < vMouseWorld.y && pos.y + decIC->sprite->height > vMouseWorld.y);
}

void IC::pressed()
{
}

void IC::UpdatePos(olc::vf2d vNewPos)
{
	olc::vf2d delta = pos - vNewPos;
	pos = vNewPos;

	for (olc::vf2d& vNodePos : vOutNodesToDraw)
		vNodePos -= delta;
	for (olc::vf2d& vNodePos : vInNodesToDraw)
		vNodePos -= delta;
}

olc::vf2d IC::GetPos()
{
	return pos;
}

bool IC::is_input()
{
	return false;
}

bool IC::is_output()
{
	return false;
}
