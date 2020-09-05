#include "Circuit.h"

template <class T>
T Max(T _t1, T _t2)
{
	if (_t1 > _t2)
		return _t1;
	return _t2;
}

void Circuit::LoadFromFile(std::string sFilename, olc::vi2d pos, SpriteManager* sm)
{
	auto Tokenize = [&](std::string sString, char token)
	{
		std::string tokenized = std::string();
		for (char c : sString)
		{
			if (c == token) return tokenized;
			tokenized += c;
		}
		return tokenized;
	};

	auto VectorContains = [&](std::vector<int> vector, int what)
	{
		for (int& elem : vector)
			if (elem == what) return true;
		return false;
	};

	std::fstream file = std::fstream(sFilename);
	std::string sLine;

	if (file.is_open())
	{
		getline(file, sLine);
		std::string sVersion = "UNDEFINED"; if (sLine.size() > 10) sVersion = sLine.substr(10);
		getline(file, sLine);

		decIC = sm->query(sLine);

		getline(file, sLine);
		sName = sLine;
		getline(file, sLine);
		sIdentifier = sLine;

		this->pos = pos;
		prepare(pos);

		std::vector<int> vOutIndicees;
		std::vector<int> vInIndicees;
		int indicee = 0;

		int outpos = 0, inpos = 0;

		while (getline(file, sLine))
		{
			if (sLine.size() > 3)
			{
				if (sLine.substr(0, 3) == "ic ")
				{
					try
					{
						// Get the information
						std::string sICLine = sLine.substr(3);
						std::string sClass = Tokenize(sICLine, ' '); sICLine = sICLine.substr(sClass.size() + 1);
						std::string sXCoord = Tokenize(sICLine, ','); sICLine = sICLine.substr(sXCoord.size() + 1);
						std::string sYCoord = sICLine;
						olc::vi2d pos = { atoi(sXCoord.c_str()), atoi(sYCoord.c_str()) };

						// Create the object
						IC* ic = nullptr;
						bool bDoPush_Back = true;
						if (sClass == "AND") ic = new AndGate(pos, sm);
						else if (sClass == "NAND") ic = new NandGate(pos, sm);
						else if (sClass == "OR") ic = new OrGate(pos, sm);
						else if (sClass == "NOR") ic = new NorGate(pos, sm);
						else if (sClass == "XOR") ic = new XorGate(pos, sm);
						else if (sClass == "XNOR") ic = new XnorGate(pos, sm);
						else if (sClass == "BUF") ic = new BufferGate(pos, sm);
						else if (sClass == "NOT") ic = new NotGate(pos, sm);
						else if (sClass == "7SEG") ic = new SevenSegmentDisplay(pos, sm);
						else if (sClass == "SDE") ic = new SevenSegmentEncoder(pos, sm);
						else if (sClass == "clk") ic = new ClockGate(pos, sm);
						else if (sClass == "BTN")
						{
							vOutIndicees.push_back(indicee);
							ic = new ButtonGate(pos, sm);
							vInputGates.push_back(ic);
						}
						else if (sClass == "BULB")
						{
							vInIndicees.push_back(indicee);
							ic = new LightBulbGate(pos, sm);
							vOutputGates.push_back(ic);
						}
						else if (sClass.substr(0, 1) == "C") ic = new Circuit(sClass.substr(1), pos, sm);
						else std::cout << "failure" << std::endl;

						vICs.push_back(ic);
						indicee++;
					}
					catch (...)
					{
						std::cout << "Error while parsing" << std::endl;
					}
				}
				else if (sLine.substr(0, 3) == "out")
				{
					vOutNames.at(outpos) = sLine.substr(4);
					outpos++;
				}
				else if (sLine.substr(0, 3) == "in ")
				{
					vInNames.at(inpos) = sLine.substr(3);
					inpos++;
				}
				else
				{
					
					std::string sNodeLine = sLine.substr(2);
					std::string sFrom = Tokenize(sNodeLine, ' ');
					std::string sTo = sNodeLine.substr(sFrom.size() + 1);

					std::string sInt;
					olc::vi2d from, to;

					Node* node;

					sInt = Tokenize(sFrom, ':');
					from.x = atoi(sInt.c_str());
					from.y = atoi(sFrom.substr(sInt.size() + 1).c_str());

					sInt = Tokenize(sTo, ':');
					to.x = atoi(sInt.c_str());
					to.y = atoi(sTo.substr(sInt.size() + 1).c_str());

					bool bOutSet = false, bInSet = false;

					// Create the node
					node = new Node();
					node->posFrom = from.y;
					node->posTo = to.y;
					node->from = vICs.at(from.x);
					node->to = vICs.at(to.x);
					node->from->addOutNode(node->posFrom, node);
					node->to->addInNode(node->posTo, node);
				}
			}
		}
		file.close();

		int index = 0;
		for (std::string &str : vInNames)
		{
			vInputGates.at(index)->sName = str;
			index++;
		}
		index = 0;
		for (std::string &str : vOutNames)
		{
			vOutputGates.at(index)->sName = str;
			index++;
		}
	}
}

void Circuit::GenerateSprite(std::string sPath)
{
	if (tSave.joinable()) tSave.detach();
	tSave = std::thread(&Circuit::GenerateSprite_Thread, this, sPath);
}

void Circuit::GenerateSprite_Thread(std::string sPath)
{
	auto ReplaceInString = [&](std::string& s, char c, char out)
	{
		for (int i = 0; i < s.size(); i++)
		{
			if (s.at(i) == c) s.at(i) = out;
		}
		return s;
	};
	auto DrawRectInSprite = [&](olc::Sprite* sprite, olc::vi2d posFrom, olc::vi2d posTo, olc::Pixel col)
	{
		for (int y = posFrom.y; y < posTo.y; y++)
			for (int x = posFrom.x; x < posTo.x; x++)
				sprite->SetPixel({ x, y }, col);
	};

	int outputNodes = vOutputGates.size();
	int inputNodes = vInputGates.size();
	int width = 64;
	int dist = 24;
	int height = dist * (Max(outputNodes, inputNodes) + 1) + 4 * Max(outputNodes, inputNodes);
	
	int ldist = dist;
	int rdist = dist;

	if (inputNodes > outputNodes) rdist = (height - 4 * outputNodes - 24 * (outputNodes - 1)) / 2;
	if (outputNodes > inputNodes) ldist = (height - 4 * inputNodes - 24 * (inputNodes - 1)) / 2;


	olc::Sprite* sprite = new olc::Sprite(width, height);

	// Edit the Sprite
	// fill with olc::VERY_DARK_BLUE
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			sprite->SetPixel({ x, y }, olc::BLANK);
		}
	}
	// write the input nodes
	for (int i = 0; i < inputNodes; i++)
	{
		int y0 = ldist + i * (4 + dist);
		for (int y = y0; y < y0 + 4; y++)
			for (int x = 0; x < 10; x++)
				sprite->SetPixel({ x, y }, olc::BLACK);
	}
	// write the output nodes
	for (int i = 0; i < outputNodes; i++)
	{
		int y0 = rdist + i * (4 + dist);
		for (int y = y0; y < y0 + 4; y++)
			for (int x = width - 10; x < width; x++)
				sprite->SetPixel({ x, y }, olc::BLACK);
	}
	// write the frame
	DrawRectInSprite(sprite, { 10, 10 }, { width - 10, 14 }, olc::BLACK); // the upper bit
	DrawRectInSprite(sprite, { 10, height - 14 }, { width - 10, height - 10 }, olc::BLACK); // the bottom bit
	DrawRectInSprite(sprite, { 10, 14 }, { 14, height - 14 }, olc::BLACK); // the left bits
	DrawRectInSprite(sprite, { width - 14, 14 }, { width - 10, height - 14 }, olc::BLACK); // the rigth bit
	// fill the frame
	DrawRectInSprite(sprite, { 14, 14 }, { width - 14, height - 14 }, olc::Pixel(59, 67, 173));

	// some nice details
	sprite->SetPixel(10, 10, olc::BLANK); sprite->SetPixel(11, 10, olc::BLANK); sprite->SetPixel(10, 11, olc::BLANK); sprite->SetPixel(14, 14, olc::BLACK); // the left upper bit
	sprite->SetPixel(10, height - 12, olc::BLANK); sprite->SetPixel(10, height - 11, olc::BLANK); sprite->SetPixel(11, height - 11, olc::BLANK); sprite->SetPixel(14, height - 15, olc::BLACK); // left bottom bit
	sprite->SetPixel(width - 11, 10, olc::BLANK); sprite->SetPixel(width - 11, 11, olc::BLANK); sprite->SetPixel(width - 12, 10, olc::BLANK); sprite->SetPixel(width - 15, 14, olc::BLACK); // right upper bit
	sprite->SetPixel(width - 11, height - 12, olc::BLANK); sprite->SetPixel(width - 11, height - 11, olc::BLANK); sprite->SetPixel(width - 12, height - 11, olc::BLANK); sprite->SetPixel(width - 15, height - 15, olc::BLACK); // the right bottom bit

#ifdef USE_PYTHON_PNG_SAVING
	// write out
	system(("TYPE NUL > " + sPath + ".pd").c_str());
	std::fstream file = std::fstream(sPath + ".pd");

	if (file.is_open())
	{
		for (int y = 0; y < sprite->height; y++)
		{
			for (int x = 0; x < sprite->width; x++)
			{
				olc::Pixel col = sprite->GetPixel(x, y);
				// write "(r,g,b, a)"
				file << "(" << (int)col.r << "," << (int)col.g << "," << (int)col.b << "," << (int)col.a << ")";
				// if its not the last Pixel in the line, write ";", if write "\n"
				if (x != sprite->width - 1)
					file << ";";
				else
					file << "\n";
			}
		}

		file.close();

		// Launch the python program
		std::string sCommand = "python generate_png.py " + sPath + ".pd " + sPath + ".png";
		system(sCommand.c_str());
		// delete the temporary file
		sCommand = "del " + ReplaceInString(sPath, '/', '\\') + ".pd";
		system(sCommand.c_str());
	}
#else
	sprite->SaveToPGESprFile(sPath + ".pgepng");
#endif
}

void Circuit::SaveToFile()
{
	auto ReplaceInString = [&](std::string& s, char token, char repl)
	{
		for (int i = 0; i < s.size(); i++)
			if (s.at(i) == token) s.at(i) = repl;
		return s;
	};

	std::string sCircuitName = ReplaceInString(sName, ' ', '-');

	std::string sPath = "circuits/" + sCircuitName;
	GenerateSprite(sPath);
	std::string sFilename = sPath + ".ic";

	system(("TYPE NUL > " + sFilename).c_str());
	std::fstream file = std::fstream(sFilename);

	if (file.is_open())
	{
		// Write the header
		file << "# VERSION 1.0f" << std::endl; // The Version
#ifdef USE_PYTHON_PNG_SAVING
		file << sPath + ".png" << std::endl; // PNG File
#else
		file << sPath + ".pgepng" << std::endl; // olc::PixelGameEngine PNG file
#endif

		file << sName << std::endl;
		file << sIdentifier << std::endl;

		std::string sLine;
		
		std::sort(vICs.begin(), vICs.end(), [](IC* a, IC* b){return a->pos.y < b->pos.y;});

		for (IC*& ic : vICs)
		{
			if (ic->is_input())
			{
				file << "in " << ((ic->sName != "Toggle Button") ? ic->sName : "") << std::endl;
			}
			else if (ic->is_output())
			{
				file << "out " << ((ic->sName != "Ligth Bulb") ? ic->sName : "") << std::endl;
			}
		}

		// Save the ICs
		for (IC* ic : vICs)
		{
			sLine = "ic ";
			if (ic->sIdentifier != "IC")
				sLine += ic->sIdentifier;
			else
			{
				Circuit* c = reinterpret_cast<Circuit*>(ic);
				if (c->sPath != sFilename) sLine += "C" + c->sPath;
			}

			sLine += " ";
			sLine += std::to_string(int(ic->pos.x));
			sLine += ",";
			sLine += std::to_string(int(ic->pos.y));

			file << sLine << std::endl;
		}

		std::vector<Node*> vNodesToSave;

		auto inVector = [&](std::vector<Node*> vNodes, Node* node)
		{
			for (Node* nNode : vNodes)
				if (nNode == node)
					return true;
			return false;
		};
		auto getIndex = [&](std::vector<IC*> vICs, IC* ic)
		{
			int i = 0;
			for (IC* icIC : vICs)
			{
				if (icIC == ic)
					return i;
				i++;
			}
			return -1;
		};



		for (IC* ic : vICs)
		{
			for (Node* node : ic->vInNodes)
			{
				if (node != nullptr)
				{
					if (!inVector(vNodesToSave, node))
						vNodesToSave.push_back(node);
				}
			}
			
			for (std::vector<Node*>& vNodes : ic->vOutNodes)
			{
				for (Node* node : vNodes)
				{
					if (node != nullptr)
					{
						if (!inVector(vNodesToSave, node))
							vNodesToSave.push_back(node);
					}
				}
			}
		}

		for (Node* node : vNodesToSave)
		{
			sLine = "n ";
			sLine += std::to_string(getIndex(vICs, node->from));
			sLine += ":";
			sLine += std::to_string(node->posFrom);
			sLine += " ";
			sLine += std::to_string(getIndex(vICs, node->to));
			sLine += ":";
			sLine += std::to_string(node->posTo);
			file << sLine << std::endl;
		}

		file.close();
	}
}

Circuit::Circuit()
{
}

Circuit::Circuit(std::string sFilename, olc::vi2d pos, SpriteManager *sm)
{
	// just load from file
	sPath = sFilename;
	load_sprites("", sm);
	LoadFromFile(sFilename, pos, sm);
}

Circuit::~Circuit()
{
	if (tSave.joinable()) tSave.join();
}

void Circuit::Update()
{
	// Get the inputs
	int i = 0;
	for (IC* ic : vInputGates)
	{
		ButtonGate* gate = reinterpret_cast<ButtonGate*>(ic);
		bool bValue;
		if (vInNodes.at(i) != nullptr)
			bValue = vInNodes.at(i)->bCharge;
		else
			bValue = false;
		gate->bPressed = bValue;
		i++;
	}

	// update the circuit
	for (IC* ic : vICs)
		if (ic != nullptr)
			ic->Update();

	// set the output
	i = 0;
	for (IC* ic : vOutputGates)
	{
		bool charge = reinterpret_cast<LightBulbGate*>(ic)->bCharge;

		for (Node* node : vOutNodes.at(i))
			if (node != nullptr)
				node->bCharge = charge;

		i++;
	}
}

void Circuit::save()
{
	SaveToFile();
}
