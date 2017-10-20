﻿#include "OverlayStartup.hpp"

#include "../devices/Display.hpp"
#include "../events/Events.hpp"
#include "../events/EventTime.hpp"
#include "../visual/DriverSlide.hpp"

namespace Flounder
{
	OverlayStartup::OverlayStartup(UiObject *parent) :
		UiObject(parent, Vector2(0.5f, 0.5f), Vector2(1.0f, 1.0f)),
		m_guiBackground(new Gui(this, Vector2(0.5f, 0.5f), Vector2(1.0f, 1.0f), new Texture("res/guis/eg_background.png"), 1)),
		m_guiLogo(new Gui(this, Vector2(0.5f, 0.5f), Vector2(0.4f, 0.4f), new Texture("res/guis/equilibrium_games.png"), 1)),
		m_textCopyright(new Text(this, Vector2(0.5f, 0.82f), "Copyright (C) 2017, Equilibrium Games - All Rights Reserved. This product uses GLFW, Vulkan, and STB Image.", 1.5f, Uis::Get()->m_cafeFrancoise, 0.5f, AlightCentre)),
		m_starting(true)
	{
		SetInScreenCoords(false);

		m_guiBackground->SetInScreenCoords(true);
		m_guiLogo->SetInScreenCoords(true);
		m_textCopyright->SetTextColour(Colour("#ffffff"));

		Events::Get()->AddEvent(new EventTime(1.649f, false, [&]()
		{
		//	SetAlphaDriver(new DriverSlide(1.0f, 0.0f, 1.0f));
		}));
	}

	OverlayStartup::~OverlayStartup()
	{
		delete m_guiBackground;
		delete m_guiLogo;
		delete m_textCopyright;
	}

	void OverlayStartup::UpdateObject()
	{
		m_guiBackground->GetDimensions()->m_x = Display::Get()->GetAspectRatio();
		m_guiBackground->SetVisible(true);
		m_guiLogo->SetVisible(true);
	}
}
