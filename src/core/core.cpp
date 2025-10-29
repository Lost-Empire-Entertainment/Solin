//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <filesystem>
#include <string>
#include <vector>
#include <sstream>

#include "KalaHeaders/log_utils.hpp"

#include "KalaWindow/include/core/core.hpp"
#include "KalaWindow/include/core/crash.hpp"
#include "KalaWindow/include/ui/font.hpp"
#include "KalaWindow/include/ui/import_kfont.hpp"

#include "core/core_program.hpp"
#include "graphics/render.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::CrashHandler;
using KalaWindow::UI::Font;
using KalaFont::GlyphResult;

using Solin::Graphics::Render;

using std::filesystem::path;
using std::filesystem::current_path;
using std::string;
using std::vector;
using std::ostringstream;

namespace Solin::Core
{
	void SolinCore::Initialize()
	{
		CrashHandler::Initialize(
			"Solin IDE",
			Shutdown);
			
		path fontPath = current_path() / "files" / "fonts" / "bw.kfont";
		
		string fontName = "bitwise";
		Font* font = Font::LoadFont(fontName, fontPath.string());
		
		if (font)
		{
			vector<GlyphResult>& result = font->GetGlyphData();
			
			ostringstream oss{};
			
			oss << "there are '" << result.size() << "' glyphs in font '" << fontName << "'\n";
			
			for (int i = 0; i < result.size(); ++i)
			{
				oss << "[" << i << "] vertices: " << result[i].vertices.size() 
				<< " indices: " << result[i].indices.size() << " indices\n";
			}
			
			Log::Print(oss.str());
		}
		
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