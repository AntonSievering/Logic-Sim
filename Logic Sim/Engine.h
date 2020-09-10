#pragma once

//#define USE_PYTHON_PNG_SAVING

#include "PGEGui.h"
#include "ICX.h"
#include "Circuit.h"
#include <thread>
#include <chrono>
#include <filesystem>
#include "SpriteManager.h"

class Engine : public PGEGui
{
private:
	enum place_t
	{
		MAINSCREEN = 0,
		OPTIONS    = 1,
		WORKING    = 2
	};
	enum mode_t
	{
		M_VIEWING = 0,
		M_ADDING = 1,
		M_WIRING = 2,
		M_DELETING = 3,
		M_ELSE = 4
	};

private:
	std::string sVersion = "1.61f";

#ifdef USE_PYTHON_PNG_SAVING
	std::string sSavingType = "Python";
#else
	std::string sSavingType = "PGE";
#endif

	// Simulation variables
	std::thread tUpdater;
	double fSimulationTime;
	bool bUpdate = true;
	bool bRunSim = true;
	bool bUpdating = false;
	bool bDebug = false;
	olc::Decal* decDebug = nullptr;

	Node* pTarget = nullptr;
	int nTargeted = -1;

	uint8_t nState = MAINSCREEN;
	uint8_t nMode = M_VIEWING;

	// buttons to cycle between modes
	Button* btnStartViewinig = nullptr;
	Button* btnStartAdding   = nullptr;
	Button* btnStartWiring   = nullptr;
	Button* btnStartDeleting = nullptr;

	// stores all ICs
	std::vector<IC*> vIC;

	// user interfaces to add ICs
	Button* btnAddPreset = nullptr;
	Button* btnAddCustom = nullptr;
	UserInterface uiAddPreset;
	UserInterface uiAddCustom;
	
	// used to save progress
	UserInterface uiSave;
	std::string sSaveFilename = "";

	// a user interface that allowes the user to rename the pRenameTarget
	UserInterface uiRename;

	// remember the target beeing moved or renamed
	IC* pMoveTarget = nullptr;
	IC* pRenameTarget = nullptr;

	// fps control
	float fTimeToNextFPSUpdate = 0.0f;
	int fps = 0;
	float fRendererTime = 0.0f;

	// Mainscreen
	Button *btnExit = nullptr;
	Button *btnOptions = nullptr;
	Button *btnLoad = nullptr;
	Button *btnNew = nullptr;
	olc::Decal *pBackground = nullptr;

	SpriteManager *spr;

	// Options
	Button *btnClose;

public:
	Engine()
	{
		sAppName = "Logic Sim";
	}

private:
	void UpdateICs()
	{
		while (bRunSim)
		{
			if (bUpdate)
			{
				bUpdating = true;
				// The start point for the time messurement
				auto tStart = std::chrono::system_clock::now();

				// Update ever IC
				for (IC* ic : vIC)
				{
					ic->Update();
				}

				// The end point for the time messurement
				auto tEnd = std::chrono::system_clock::now();
				size_t nNanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>
					(tEnd - tStart).count();
				fSimulationTime = (double)nNanoseconds / 1000'000.0;

				bUpdating = false;
			}
			else std::this_thread::sleep_for(std::chrono::microseconds(100));
		}
	}

	void DrawDebugWindow(float fElapsedTime)
	{
		// Draw the background
		DrawDecal({ 0, 0 }, decDebug, { 150.0f, 80.0f });

		// Draw the title
		DrawStringDecal({ 5, 5 }, "Debug Information:");

		// Draw the Current Mode
		std::string sMode = "";
		switch (nMode)
		{
		case M_VIEWING: sMode = "Viewing"; break;
		case M_ADDING: sMode = "Adding"; break;
		case M_WIRING: sMode = "Wiring"; break;
		case M_DELETING: sMode = "Deleting"; break;
		default: sMode = "ERROR"; break;
		}
		DrawStringDecal({ 5, 20 }, "Mode: " + sMode);

		// Draw the taken time to update the simulation
		DrawStringDecal({ 5, 35 }, "Tsim: " + std::to_string(fSimulationTime) + "ms");

		// Draw the number of current ICs
		DrawStringDecal({ 5, 50 }, "Number of ICs: " + std::to_string(vIC.size()));

		// Draw the fps and rendering time
		fTimeToNextFPSUpdate += fElapsedTime;
		if (fTimeToNextFPSUpdate > 0.5f)
		{
			fTimeToNextFPSUpdate -= 0.5f;
			fps = (int)round(1.0f / fElapsedTime);
			fRendererTime = 1000 * fElapsedTime;
		}
		DrawStringDecal({ 5, 65 }, std::to_string(fps) + "fps / " + std::to_string(fRendererTime) + "ms");
	}

	void SaveICs()
	{
		Circuit c;
		c.sName = sSaveFilename;
		c.vICs = vIC;

		for (IC* ic : vIC)
		{
			// if the ic is a button
			if (ic->is_input())
			{
				c.vInputGates.push_back(ic);
			}
			// if the ic is a ligth bulb
			else if (ic->is_output())
			{
				c.vOutputGates.push_back(ic);
			}
		}

		c.save();
	}

	void load_custom_entries()
	{
		// delete all widgets
		for (auto& widget : uiAddCustom.vWidgets)
			delete widget;
		uiAddCustom.vWidgets = std::vector<Widget*>();

		int entries = 0;
		for (const auto& entry : std::filesystem::directory_iterator("circuits"))
			entries++;

		Slider* sld = new Slider(false, { 750, 0 }, { 50, 720 }, (size_t)100 * entries, olc::BLANK, olc::GREY);
		uiAddCustom.Add(sld);

		// load all custom ICs
		int y = 0;
		for (const auto& entry : std::filesystem::directory_iterator("circuits"))
		{
			std::string str = entry.path().string();
			if (str.substr(str.size() - 2, 2) == "ic")
			{
				std::string sLine;
				std::fstream file = std::fstream(str);
				if (file.is_open())
				{
					getline(file, sLine);
					getline(file, sLine);
					getline(file, sLine);
				}

				Button* btn = new Button({ 0, y }, { 750, 100 }, sLine, olc::BLACK);
				btn->sHint = str;

				uiAddCustom.Add(btn);
				y += 100;
			}
		}
	}

	void load_Circuit(const std::string& sFilename)
	{
		Circuit c = Circuit(sFilename, { 0, 0 }, spr);
		vIC = c.vICs;
	}

	bool OnUserMainscreen(float fElapsedTime)
	{
		Clear(olc::VERY_DARK_BLUE);

		if (btnExit->isPressed(this))
		{
			return false;
		}
		if (btnOptions->isPressed(this))
		{
			nState = OPTIONS;
			return true;
		}
		if (btnNew->isPressed(this))
		{
			nState = WORKING;
			uiAddCustom.lblName.Update_Text("Add Component");
			tUpdater = std::thread(&Engine::UpdateICs, this);

			return true;
		}
		if (btnLoad->isPressed(this))
		{
			load_custom_entries();
			uiAddCustom.bIsVisible = true;
			uiAddCustom.lblName.Update_Text("Load IC");
		}

		if (uiAddCustom.bIsVisible)
		{
			for (Widget *widget : uiAddCustom.vWidgets)
			{
				if (widget->is_button())
				{
					Button *btn = reinterpret_cast<Button *>(widget);
					if (btn->isPressed(this))
					{
						std::string sName = btn->sHint.substr(0, btn->sHint.size() - 3);
						load_Circuit(btn->sHint);
						sSaveFilename = sName.substr(9);
						uiAddCustom.lblName.Update_Text("Add Component");
						uiAddCustom.bIsVisible = false;
						tUpdater = std::thread(&Engine::UpdateICs, this);

						nState = WORKING;
						return true;
					}
				}
			}
		}

		btnExit->bVisible = btnOptions->bVisible = btnNew->bVisible = btnLoad->bVisible = !uiAddCustom.bIsVisible;

		if (!uiAddCustom.bIsVisible)
			DrawDecal({ 0, 0 }, pBackground);
		uiAddCustom.Draw(this);

		// Draw the buttons
		btnExit->Draw(this);
		btnOptions->Draw(this);
		btnLoad->Draw(this);
		btnNew->Draw(this);

		return true;
	}

	bool OnUserInformation(float fElapsedTime)
	{
		Clear(olc::VERY_DARK_BLUE);

		if (btnClose->isPressed(this))
		{
			nState = MAINSCREEN;
			return true;
		}

		btnClose->Draw(this);

		// Draw some text
		DrawStringDecal({ 100, 100 }, "This project was compiled using the\nlastes C++ standard but it should\nwork with C++17 as well.", olc::GREY, { 2.0f, 2.0f });
		DrawStringDecal({ 100, 150 }, "It uses the olc::PixelGameEngine\nby javidx9.", olc::GREY, { 2.0f, 2.0f });
		DrawStringDecal({ 100, 200 }, "For help consider checking out my\nGithub repository where you will find\ninformation on controls, shortcuts\nand tricks.\n(https://github.com/AntonSievering/\nLogic-Sim)", olc::GREY, { 2.0f, 2.0f });

		// Draw the type
		DrawStringDecal({   0, 780 }, "Saving type: " + sSavingType, olc::BLACK, { 2.0f, 2.0f });
		// draw the version
		DrawStringDecal({ 575, 780 }, "Version: "     + sVersion,    olc::BLACK, { 2.0f, 2.0f });

		return true; 
	}

	bool OnUserWorking(float fElapsedTime)
	{
		if (sSaveFilename == "")
			sAppName = "Logic Sim - Untitled";
		else
			sAppName = "Logic Sim - " + sSaveFilename;

		bUpdate = true;
		Clear(olc::VERY_DARK_BLUE);
		olc::vf2d vMouse = olc::vf2d(GetMouseX(), GetMouseY());
		UpdatePanAndZoom(fElapsedTime, !(uiSave.bIsVisible || uiRename.bIsVisible));

		if (!(uiSave.bIsVisible || uiRename.bIsVisible) && GetKey(olc::Key::ESCAPE).bPressed)
		{
			// stop the thread
			if (tUpdater.joinable())
			{
				bRunSim = false;
				tUpdater.join();
				bRunSim = true;
			}

			if (sSaveFilename != "")
				SaveICs();
			sSaveFilename = "";

			for (IC *ic : vIC)
				delete ic;
			vIC.clear();

			nState = MAINSCREEN;
			return true;
		}

		// toggle Debug Mode
		if (GetKey(olc::Key::F1).bPressed) bDebug = !bDebug;

		bool bButtonPressed = false;

		if (btnStartAdding->isPressed(this))
		{
			nMode = M_ADDING;
			bButtonPressed = true;
		}
		else if (btnStartDeleting->isPressed(this))
		{
			nMode = M_DELETING;
			bButtonPressed = true;
		}
		else if (btnStartViewinig->isPressed(this))
		{
			nMode = M_VIEWING;
			bButtonPressed = true;
		}
		else if (btnStartWiring->isPressed(this))
		{
			nMode = M_WIRING;
			bButtonPressed = true;
		}

		// Saving
		bool bFlushSaveStringBuffer = false;
		bool bFlushRenameStringBuffer = false;
		if (GetKey(olc::Key::S).bPressed && GetKey(olc::Key::SHIFT).bHeld && GetKey(olc::Key::CTRL).bHeld)
		{
			sSaveFilename = "";
		}

		if (GetKey(olc::Key::S).bPressed && GetKey(olc::Key::CTRL).bHeld)
		{
			if (sSaveFilename != "")
			{
				SaveICs();
				load_custom_entries();
			}
			else
			{
				uiSave.Show();
				// reset the input widget
				bFlushSaveStringBuffer = true;
			}
		}

		// cancel button
		Button* btn = reinterpret_cast<Button*>(uiSave.vWidgets.at(1));
		if (btn->isPressed(this))
		{
			uiSave.Vanish();
			bFlushSaveStringBuffer = true;
		}

		// save button
		btn = reinterpret_cast<Button*>(uiSave.vWidgets.at(2));
		if (uiSave.bIsVisible && btn->isPressed(this))
		{
			sSaveFilename = reinterpret_cast<InputWidget*>(uiSave.vWidgets.at(0))->sText;
			if (sSaveFilename != "")
			{
				SaveICs();
				load_custom_entries();
				uiSave.Vanish();
				bFlushSaveStringBuffer = true;
			}
		}

		// user input
		if (GetKey(olc::Key::K1).bHeld && !(uiSave.bIsVisible || uiRename.bIsVisible))
		{
			nMode = M_VIEWING;
			btnAddPreset->bVisible = btnAddCustom->bVisible = false;
		}
		if (GetKey(olc::Key::K2).bHeld && !(uiSave.bIsVisible || uiRename.bIsVisible))
		{
			nMode = M_ADDING;
			btnAddPreset->bVisible = btnAddCustom->bVisible = true;
		}
		if (GetKey(olc::Key::K3).bHeld && !(uiSave.bIsVisible || uiRename.bIsVisible))
		{
			nMode = M_WIRING;
			btnAddPreset->bVisible = btnAddCustom->bVisible = false;
		}
		if (GetKey(olc::Key::K4).bHeld && !(uiSave.bIsVisible || uiRename.bIsVisible))
		{
			nMode = M_DELETING;
			btnAddPreset->bVisible = btnAddCustom->bVisible = false;
		}
		if (nMode != M_WIRING) pTarget = nullptr;

		// WIRING MODE
		if (GetMouse(0).bPressed && nMode == M_WIRING && !bButtonPressed)
		{
			// select an output node
			bool bSelected = false;
			for (auto ic : vIC)
			{
				int nSelected = ic->getSelectedOutNode(this);
				if (nSelected != -1)
				{
					delete pTarget;
					pTarget = new Node(ic, nullptr, nSelected, -1);

					bSelected = true;
					nTargeted = nSelected;
				}
			}

			// if the target exists
			if (pTarget != nullptr)
			{
				// select an input node
				for (IC* ic : vIC)
				{
					int nSeleted = ic->getSelectedInNode(this);
					if (nSeleted != -1 && !ic->getInNodeState(nSeleted))
					{
						pTarget->to = ic;
						pTarget->posTo = nSeleted;

						pTarget->to->addInNode(pTarget->posTo, pTarget);
						pTarget->from->addOutNode(pTarget->posFrom, pTarget);
						
						pTarget = new Node(pTarget->from, nullptr, pTarget->posFrom, -1);

						bSelected = true;
					}
				}

				if (!bSelected)
				{
					delete pTarget; pTarget = nullptr;
					nTargeted = -1;
				}
			}
		}
		// VIEWING MODE
		if (nMode == M_VIEWING)
		{
			if (GetMouse(0).bPressed)
			{
				// Check which one has been pressed
				for (IC* ic : vIC)
				{
					if (ic->collides_with(this, vMouse))
						ic->pressed();
				}
			}
		}
		// DELETING MODE
		if (nMode == M_DELETING)
		{
			if (GetMouse(0).bPressed)
			{
				// delete ICs
				int iterator = 0;
				for (IC* ic : vIC)
				{
					if (ic->collides_with(this, vMouse))
					{
						// Stop the Updater Thread
						if (tUpdater.joinable())
						{
							bRunSim = false;
							tUpdater.join();
						}

						std::vector<Node*> vInNodes = *ic->getInNodes();
						for (Node *node : vInNodes)
						{
							if (node != nullptr)
							{
								node->from->deleteOutNode(node->posFrom, node);
								delete node;
								node = nullptr;
							}
						}

						std::vector<std::vector<Node*>> vOutNodes = *ic->getOutNodes();
						for (std::vector<Node *> &vNodes : vOutNodes)
						{
							for (Node *node : vNodes)
							{
								if (node != nullptr)
								{
									node->to->deleteInNode(node->posTo);
									delete node;
									node = nullptr;
								}
							}
						}

						delete ic;
						vIC.erase(vIC.begin() + iterator);

						// Restart the Updater Thread
						bRunSim = true;
						tUpdater = std::thread(&Engine::UpdateICs, this);

						goto DELETE_END;
					}
					iterator++;
				}

				// delete Nodes
				for (IC *ic : vIC)
				{
					int n = ic->getSelectedInNode(this);
					if (n != -1)
					{
						Node *node = ic->vInNodes.at(n);
						if (node != nullptr)
						{
							node->from->deleteOutNode(node->posFrom, node);
							ic->deleteInNode(n);
							delete node;
						}
					}

					n = ic->getSelectedOutNode(this);
					if (n != -1)
					{
						for (Node *node : ic->vOutNodes.at(n))
						{
							if (node != nullptr)
							{
								ic->deleteOutNode(node->posFrom, node);
								node->to->deleteInNode(node->posTo);
								delete node;
								node = nullptr;
							}
						}
					}
				}
			}
		}
		// ADDING MODE
		if (nMode == M_ADDING)
		{
			bool btnPressed = false;
			if (btnAddPreset->isPressed(this))
			{
				uiAddPreset.Show();
				btnPressed = true;
				bButtonPressed = true;
			}
			if (btnAddCustom->isPressed(this))
			{
				uiAddCustom.Show();
				btnPressed = true;
				bButtonPressed = true;
			}
			bool btnVisible = !(uiAddCustom.bIsVisible || uiAddPreset.bIsVisible);
			btnAddCustom->bVisible = btnAddPreset->bVisible = btnVisible;

			if (uiAddPreset.bIsVisible && !btnPressed && !bButtonPressed)
			{
				for (Widget* widget : uiAddPreset.vWidgets)
				{
					if (widget->is_button())
					{
						Button* btn = reinterpret_cast<Button*>(widget);
						if (btn->isPressed(this) && !reinterpret_cast<Slider*>(uiAddPreset.vWidgets.at(0))->bStillPressed)
						{
							if (tUpdater.joinable())
							{
								bRunSim = false;
								tUpdater.join();
								bRunSim = true;

								if (btn->sText == "Not Gate")          vIC.push_back(new NotGate            (ScreenToWorld({ 400, 400 }), spr));
								if (btn->sText == "Buffer Gate")       vIC.push_back(new BufferGate         (ScreenToWorld({ 400, 400 }), spr));
								if (btn->sText == "And Gate")          vIC.push_back(new AndGate            (ScreenToWorld({ 400, 400 }), spr));
								if (btn->sText == "Nand Gate")         vIC.push_back(new NandGate           (ScreenToWorld({ 400, 400 }), spr));
								if (btn->sText == "Or Gate")           vIC.push_back(new OrGate             (ScreenToWorld({ 400, 400 }), spr));
								if (btn->sText == "Nor Gate")          vIC.push_back(new NorGate            (ScreenToWorld({ 400, 400 }), spr));
								if (btn->sText == "Xor Gate")          vIC.push_back(new XorGate            (ScreenToWorld({ 400, 400 }), spr));
								if (btn->sText == "Xnor Gate")         vIC.push_back(new XnorGate           (ScreenToWorld({ 400, 400 }), spr));
								if (btn->sText == "Button")            vIC.push_back(new ButtonGate         (ScreenToWorld({ 400, 400 }), spr));
								if (btn->sText == "Light Bulb")        vIC.push_back(new LightBulbGate      (ScreenToWorld({ 400, 400 }), spr));
								if (btn->sText == "7 Segment Display") vIC.push_back(new SevenSegmentDisplay(ScreenToWorld({ 400, 400 }), spr));
								if (btn->sText == "7 Segment Encoder") vIC.push_back(new SevenSegmentEncoder(ScreenToWorld({ 400, 400 }), spr));
								if (btn->sText == "Clock")             vIC.push_back(new ClockGate          (ScreenToWorld({ 400, 400 }), spr));

								bRunSim = true;
								tUpdater = std::thread(&Engine::UpdateICs, this);
							}
						}
					}
				}
			}

			if (uiAddCustom.bIsVisible && !btnPressed && !bButtonPressed)
			{
				for (Widget* widget : uiAddCustom.vWidgets)
				{
					if (widget->is_button())
					{
						Button* btn = reinterpret_cast<Button*>(widget);
						if (btn->isPressed(this))
						{
							if (tUpdater.joinable())
							{
								bRunSim = false;
								tUpdater.join();
								bRunSim = true;

								vIC.push_back(new Circuit(btn->sHint, ScreenToWorld({ 400, 400 }), spr));

								tUpdater = std::thread(&Engine::UpdateICs, this);
							}
						}
					}
				}
			}

			if (GetMouse(0).bPressed)
			{
				bool bCollided = false;
				for (IC* ic : vIC)
				{
					if (pMoveTarget != ic && ic->collides_with(this, vMouse))
					{
						pMoveTarget = ic;
						bCollided = true;
					}
				}
				if (!bCollided)
				{
					pMoveTarget = nullptr;
				}
				// sort the MoveTarget to the end of the drawing pipeline
				else
				{
					int index = 0;
					for (int i = 0; i < vIC.size(); i++)
						if (vIC.at(i) == pMoveTarget)
						{
							index = i;
							break;
						}

					vIC.erase(vIC.begin() + index);
					vIC.push_back(pMoveTarget);
				}
			}
			if (pMoveTarget != nullptr)
			{
				pMoveTarget->UpdatePos(ScreenToWorld(vMouse) - olc::vf2d(pMoveTarget->decIC->sprite->width, pMoveTarget->decIC->sprite->height) / 2);
				if (GetKey(olc::Key::R).bPressed)
				{
					uiRename.bIsVisible = true;
					bFlushRenameStringBuffer = true;
					pRenameTarget = pMoveTarget;
					pMoveTarget = nullptr;
				}
			}

			if (GetKey(olc::Key::ESCAPE).bPressed)
			{
				uiAddPreset.Vanish();
				uiAddCustom.Vanish();
			}
		}

DELETE_END:

		bool bUIVisible = (uiAddCustom.bIsVisible || uiAddPreset.bIsVisible);
		btnAddPreset->bVisible = (nMode == M_ADDING && !bUIVisible);
		btnAddCustom->bVisible = (nMode == M_ADDING && !bUIVisible);
		btnStartAdding->bVisible = !bUIVisible;
		btnStartDeleting->bVisible = !bUIVisible;
		btnStartViewinig->bVisible = !bUIVisible;
		btnStartWiring->bVisible = !bUIVisible;

		// draw all ICs
		if (uiAddPreset.bIsVisible)
		{
			uiAddPreset.Draw(this);
		}
		else if (uiAddCustom.bIsVisible)
		{
			uiAddCustom.Draw(this);
		}
		else
		{
			for (IC* ic : vIC)
			{
				if (pTarget != nullptr && pTarget->from == ic)
					ic->Draw(this, nTargeted);
				else ic->Draw(this, -1);
			}
		}

		if (uiSave.bIsVisible)
		{
			uiSave.Draw(this);
		}
		else if (uiRename.bIsVisible)
		{
			// Cancel Button
			if (reinterpret_cast<Button*>(uiRename.vWidgets.at(1))->isPressed(this))
			{
				uiRename.bIsVisible = false;
				bFlushRenameStringBuffer = true;
			}
			// Save Button
			else if (reinterpret_cast<Button*>(uiRename.vWidgets.at(2))->isPressed(this))
			{
				std::string s = reinterpret_cast<InputWidget*>(uiRename.vWidgets.at(0))->sText;
				pRenameTarget->sName = s;
				bFlushRenameStringBuffer = true;
				uiRename.bIsVisible = false;
			}
			uiRename.Draw(this);
		}

		if (bFlushSaveStringBuffer)
			reinterpret_cast<InputWidget*>(uiSave.vWidgets.at(0))->reset();
		if (bFlushRenameStringBuffer)
			reinterpret_cast<InputWidget*>(uiRename.vWidgets.at(0))->sText = "";

		btnAddPreset->Draw(this);
		btnAddCustom->Draw(this);

		btnStartAdding->Draw(this);
		btnStartDeleting->Draw(this);
		btnStartViewinig->Draw(this);
		btnStartWiring->Draw(this);

		std::string sMode;
		switch (nMode)
		{
		case M_ADDING: sMode = "adding"; break;
		case M_VIEWING: sMode = "viewing"; break;
		case M_DELETING: sMode = "deleting"; break;
		case M_WIRING: sMode = "wiring"; break;
		}

		if (!bUIVisible)
			DrawStringDecal({ 0, 80 }, "current mode: " + sMode, olc::GREY, { 2.0f, 2.0f });

		if (bDebug)
			DrawDebugWindow(fElapsedTime);

		return true;
	}

private:
	virtual bool OnUserCreate() override
	{
		// Standard Zoom Settings
		fZoom = 1.0f;
		panOffset = panStart = { 0.0f, 0.0f };
		vZoomBorder = { 0.1, 5.0 };

		spr = new SpriteManager();
		spr->load_dir("sprites");
		spr->load_dir("circuits");

		// The Background of the Debug window
		olc::Sprite* spr = new olc::Sprite(1, 1);
		spr->SetPixel(0, 0, olc::VERY_DARK_GREY);
		decDebug = new olc::Decal(spr);

		btnStartViewinig = new Button({   0, 0 }, { 200, 75 }, "Viewing Mode",  olc::BLACK);
		btnStartAdding   = new Button({ 200, 0 }, { 200, 75 }, "Adding Mode",   olc::BLACK);
		btnStartWiring   = new Button({ 400, 0 }, { 200, 75 }, "Wiring Mode",   olc::BLACK);
		btnStartDeleting = new Button({ 600, 0 }, { 200, 75 }, "Deleting Mode", olc::BLACK);

		pTarget = nullptr;

		// set the button background
		vButton.push_back(new olc::Decal(new olc::Sprite("sprites/buttonBase.png")));
		vButton_Hover.push_back(new olc::Decal(new olc::Sprite("sprites/buttonHovered.png")));
		vButton_Pressed.push_back(new olc::Decal(new olc::Sprite("sprites/buttonPressed.png")));

		// the two adding buttons
		btnAddPreset = new Button({ 0, 725 }, { 200, 75 }, "Add Preset IC", olc::GREY);
		btnAddPreset->bVisible = false;
		btnAddCustom = new Button({ 200, 725 }, { 200, 75 }, "Add Custom IC", olc::GREY);
		btnAddCustom->bVisible = false;

		// load all preset ICs
		uiAddPreset = UserInterface({ 0, 0 }, { 800, 800 }, new olc::Decal(new olc::Sprite()), "Add Component");
		Slider* sld = new Slider(false, { 750, 0 }, { 50, 720 }, 725, olc::BLANK, olc::GREY);
		uiAddPreset.Add(sld);
		uiAddPreset.Add(new Button({ 0,    0 }, { 750, 100 }, "Not Gate",          olc::BLACK, sld));
		uiAddPreset.Add(new Button({ 0,  100 }, { 750, 100 }, "Buffer Gate",       olc::BLACK, sld));
		uiAddPreset.Add(new Button({ 0,  200 }, { 750, 100 }, "And Gate",          olc::BLACK, sld));
		uiAddPreset.Add(new Button({ 0,  300 }, { 750, 100 }, "Nand Gate",         olc::BLACK, sld));
		uiAddPreset.Add(new Button({ 0,  400 }, { 750, 100 }, "Or Gate",           olc::BLACK, sld));
		uiAddPreset.Add(new Button({ 0,  500 }, { 750, 100 }, "Nor Gate",          olc::BLACK, sld));
		uiAddPreset.Add(new Button({ 0,  600 }, { 750, 100 }, "Xor Gate",          olc::BLACK, sld));
		uiAddPreset.Add(new Button({ 0,  700 }, { 750, 100 }, "Xnor Gate",         olc::BLACK, sld));
		uiAddPreset.Add(new Button({ 0,  800 }, { 750, 100 }, "Button",            olc::BLACK, sld));
		uiAddPreset.Add(new Button({ 0,  900 }, { 750, 100 }, "Light Bulb",        olc::BLACK, sld));
		uiAddPreset.Add(new Button({ 0, 1000 }, { 750, 100 }, "7 Segment Display", olc::BLACK, sld));
		uiAddPreset.Add(new Button({ 0, 1100 }, { 750, 100 }, "7 Segment Encoder", olc::BLACK, sld));
		uiAddPreset.Add(new Button({ 0, 1200 }, { 750, 100 }, "Clock",             olc::BLACK, sld));
		uiAddPreset.Vanish();

		uiAddCustom = UserInterface({ 0, 0 }, { 800, 800 }, new olc::Decal(new olc::Sprite()), "Add Component");
		load_custom_entries();
		uiAddCustom.Vanish();

		uiSave = UserInterface({ 150, 325 }, { 500, 190 }, decDebug, "Save as");
		uiSave.Add(new InputWidget({ 0, 0 }, { 500, 100 }, olc::BLACK, olc::BLANK)); reinterpret_cast<InputWidget*>(uiSave.vWidgets.at(0))->bCRLFAllowed = false;
		uiSave.Add(new Button({ 0, 100 }, { 250, 75 }, "Cancel", olc::BLACK));
		uiSave.Add(new Button({ 250, 100 }, { 250, 75 }, "Save", olc::BLACK));
		uiSave.Vanish();
		
		uiRename = UserInterface({ 150, 325 }, { 500, 190 }, decDebug, "Rename");
		uiRename.Add(new InputWidget({ 0, 0 }, { 500, 100 }, olc::BLACK, olc::BLANK)); reinterpret_cast<InputWidget*>(uiRename.vWidgets.at(0))->bCRLFAllowed = false;
		uiRename.Add(new Button({ 0, 100 }, { 250, 75 }, "Cancel", olc::BLACK));
		uiRename.Add(new Button({ 250, 100 }, { 250, 75 }, "Apply", olc::BLACK));
		uiRename.Vanish();

		// load mainscreen stuff
		btnExit    = new Button({ 250, 700 }, { 300, 75 }, "Exit",        olc::BLACK);
		btnOptions = new Button({ 250, 625 }, { 300, 75 }, "Information", olc::BLACK);
		btnLoad    = new Button({ 250, 550 }, { 300, 75 }, "Load",        olc::BLACK);
		btnNew     = new Button({ 250, 475 }, { 300, 75 }, "New",         olc::BLACK);

		btnClose = new Button({ 750, 0 }, { 50, 50 }, "X", olc::BLACK);
		pBackground = new olc::Decal(new olc::Sprite("sprites/mainscreenbg.png"));

		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime) override
	{
		// Just execute the State
		switch (nState)
		{
		case MAINSCREEN: return OnUserMainscreen(fElapsedTime);
		case OPTIONS: return OnUserInformation(fElapsedTime);
		case WORKING: return OnUserWorking(fElapsedTime);
		}

		return false;
	}

	virtual bool OnUserDestroy() override
	{
		// end the simulation
		bRunSim = false;
		if (tUpdater.joinable()) tUpdater.join();

		return true;
	}
};