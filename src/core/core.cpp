//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "KalaWindow/include/core/core.hpp"
#include "KalaWindow/include/core/crash.hpp"

#include "core/core_program.hpp"
#include "graphics/render.hpp"

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::CrashHandler;

using Solin::Graphics::Render;

namespace Solin::Core
{
	void SolinCore::Initialize()
	{
		CrashHandler::Initialize(
			"Solin IDE",
			Shutdown);
		
		Render::Initialize();
	}
	
	void SolinCore::Update()
	{
		while (true)
		{
			KalaWindowCore::UpdateDeltaTime();
			Render::Update();
		}
	}
	
	void SolinCore::Shutdown()
	{
		
	}
}