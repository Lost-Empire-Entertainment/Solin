//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <filesystem>
#include <vector>
#include <string>

#include "KalaHeaders/log_utils.hpp"

#include "KalaWindow/include/core/core.hpp"
#include "KalaWindow/include/core/crash.hpp"

#include "core/core_program.hpp"
#include "core/import_kfont.hpp"
#include "graphics/render.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::CrashHandler;

using KalaFont::ImportKalaFont;
using KalaFont::GlyphResult;

using Solin::Graphics::Render;

using std::filesystem::path;
using std::filesystem::current_path;
using std::vector;
using std::to_string;

namespace Solin::Core
{
	void SolinCore::Initialize()
	{
		CrashHandler::Initialize(
			"Solin IDE",
			Shutdown);
			
		path fontPath = current_path() / "files" / "fonts" / "bw.kfont";
		vector<GlyphResult> result{};
		
		if (!ImportKalaFont(fontPath, result))
		{
			KalaWindowCore::ForceClose(
				"Font error",
				"Failed to load font from path '" + fontPath.string() + "'!");
		}
		
		Log::Print(
			"Loaded '" + to_string(result.size()) + "' glyphs from font '" + fontPath.string() + "'!",
			"LOAD_KFONT",
			LogType::LOG_SUCCESS);
		
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