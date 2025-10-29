//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>
#include <vector>
#include <filesystem>

#include "KalaHeaders/log_utils.hpp"
#include "KalaHeaders/math_utils.hpp"

#include "KalaWindow/include/core/core.hpp"
#include "KalaWindow/include/core/input.hpp"
#include "KalaWindow/include/utils/registry.hpp"
#include "KalaWindow/include/utils/transform2d.hpp"
#include "KalaWindow/include/graphics/window_global.hpp"
#include "KalaWindow/include/graphics/window.hpp"
#include "KalaWindow/include/graphics/opengl/opengl.hpp"
#include "KalaWindow/include/graphics/opengl/opengl_texture.hpp"
#include "KalaWindow/include/graphics/opengl/opengl_shader.hpp"
#include "KalaWindow/include/graphics/opengl/opengl_functions_core.hpp"
#include "KalaWindow/include/graphics/opengl/shaders/shader_quad.hpp"
#include "KalaWindow/include/ui/image.hpp"

#include "graphics/render.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;
using KalaHeaders::vec2;
using KalaHeaders::vec3;
using KalaHeaders::mat4;
using KalaHeaders::ortho;

using KalaWindow::Core::KalaWindowCore;
using KalaWindow::Core::Input;
using KalaWindow::Utils::Registry;
using KalaWindow::Graphics::Window_Global;
using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::WindowState;
using KalaWindow::Graphics::OpenGL::OpenGL_Global;
using KalaWindow::Graphics::OpenGL::OpenGL_Context;
using KalaWindow::Graphics::OpenGL::OpenGL_Texture;
using KalaWindow::Graphics::TextureType;
using KalaWindow::Graphics::TextureFormat;
using KalaWindow::Graphics::OpenGL::OpenGL_Shader;
using KalaWindow::Graphics::OpenGL::ShaderData;
using KalaWindow::Graphics::OpenGL::ShaderType;
using KalaWindow::Graphics::OpenGL::VSyncState;
using KalaWindow::Graphics::OpenGL::Shader::shader_quad_vertex;
using KalaWindow::Graphics::OpenGL::Shader::shader_quad_fragment;
using namespace KalaWindow::Graphics::OpenGLFunctions;
using KalaWindow::UI::Image;

using std::string;
using std::vector;
using std::filesystem::current_path;

static void Redraw(Window* window);
static void Resize(Window* window);

static Window* CreateNewWindow(
	const string& name,
	Window* parentWindow = nullptr);
	
//light blue background color
constexpr vec3 NORMALIZED_BACKGROUND_COLOR = vec3(0.29f, 0.36f, 0.85f);

constexpr vec2 BASE_SIZE = vec2(1280.0f, 720.0f);

namespace Solin::Graphics
{
	void Render::Initialize()
	{
		Window_Global::Initialize();
		OpenGL_Global::Initialize();
		
		Window* window = CreateNewWindow("Solin IDE");

		u32 windowID = window->GetID();

		auto contexts = OpenGL_Context::registry.GetAllWindowContent(windowID);
		OpenGL_Context* context = contexts.empty() ? nullptr : contexts.front();

		if (!context)
		{
			KalaWindowCore::ForceClose(
				"Initialization error",
				"Failed to attach an OpenGL context to window '" + window->GetTitle() + "'!");

			return;
		}

		context->MakeContextCurrent();

		glEnable(GL_FRAMEBUFFER_SRGB);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);    //Cull back faces (default)
		glFrontFace(GL_CCW);    //Define CCW vertices as front-facing

#ifdef _DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); //Ensures callbacks run immediately
		glDebugMessageCallback(DebugCallback, nullptr);
#endif

		string texPath = (current_path() / "files" / "UI" / "image1.png").string();

		OpenGL_Texture* tex01 = OpenGL_Texture::LoadTexture(
			windowID,
			"tex01",
			texPath,
			TextureType::Type_2D,
			TextureFormat::Format_RGBA8);

		OpenGL_Shader* shader01 = OpenGL_Shader::CreateShader(
			windowID,
			"shader01",
			{ {
				{.shaderData = string(shader_quad_vertex), .type = ShaderType::SHADER_VERTEX },
				{.shaderData = string(shader_quad_fragment), .type = ShaderType::SHADER_FRAGMENT }
			} });

		Image* image = Image::Initialize(
			"img01",
			windowID,
			vec2(0),
			0.0f,
			vec2(256),
			nullptr,
			tex01,
			shader01);
	}
	
	void Render::Update()
	{
		for (const auto& window : Window::registry.runtimeContent)
		{
			if (!window) continue;
			
			window->Update();
			u32 windowID = window->GetID();

			const vector<Input*>& inputs = Input::registry.GetAllWindowContent(windowID);
			Input* input = inputs.empty() ? nullptr : inputs.front();

			if (!window->IsIdle()
				&& !window->IsResizing())
			{
				Redraw(window);
			}

			if (input) input->EndFrameUpdate();
		}
	}
}

void Redraw(Window* window)
{
	if (!window) return;

	u32 windowID = window->GetID();

	mat4 projection = ortho(window->GetFramebufferSize());

	const vector<OpenGL_Context*>& contexts = OpenGL_Context::registry.GetAllWindowContent(windowID);
	OpenGL_Context* context = contexts.empty() ? nullptr : contexts.front();

	if (!context) return;
	
	context->MakeContextCurrent();

	glClearColor(
		NORMALIZED_BACKGROUND_COLOR.x,
		NORMALIZED_BACKGROUND_COLOR.y,
		NORMALIZED_BACKGROUND_COLOR.z,
		1.0f);
	glClear(
		GL_COLOR_BUFFER_BIT
		| GL_DEPTH_BUFFER_BIT);

	glDisable(GL_CULL_FACE);
	const vector<Image*>& images = Image::registry.GetAllWindowContent(windowID);
	for (const auto& image : images)
	{
		if (image)
		{
			image->MoveWidget(
				window->GetClientRectSize(),
				vec2(1.0f, 1.0f));

			image->Render(projection);
		}
	}
	glEnable(GL_CULL_FACE);

	context->SwapOpenGLBuffers();
}

void Resize(Window* window)
{

}

Window* CreateNewWindow(
	const string& name,
	Window* parentWindow)
{
	Window* window = Window::Initialize(
		name,
		BASE_SIZE,
		parentWindow,
		WindowState::WINDOW_HIDE);

	if (!window)
	{
		KalaWindowCore::ForceClose(
			"Initialization error",
			"Failed to create a window!");

		return nullptr;
	}
	
	window->SetRedrawCallback([window]() { Redraw(window); });
	window->SetResizeCallback([window]() { Resize(window); });

	window->BringToFocus();

	u32 windowID = window->GetID();

	OpenGL_Context* context = OpenGL_Context::Initialize(windowID, 0);

	if (!context)
	{
		KalaWindowCore::ForceClose(
			"Initialization error",
			"Failed to attach an OpenGL context to window '" + window->GetTitle() + "'!");

		return nullptr;
	}

	context->SetVSyncState(VSyncState::VSYNC_ON);

	/*
	if (!windowIconTexture)
	{
		if (!exists(windowIconPath))
		{
			KalaWindowCore::ForceClose(
				"Initialization error",
				"Failed to attach icon to window '" + window->GetTitle() + "' because the icon was not found!");

			return nullptr;
		}
		else
		{
			windowIconTexture = OpenGL_Texture::LoadTexture(
				windowID,
				"exeIcon",
				windowIconPath.data(),
				TextureType::Type_2D,
				TextureFormat::Format_RGBA8);
		}
	}
	else window->SetIcon(windowIconTexture->GetID());
	*/

	Input* input = Input::Initialize(windowID);

	if (!input)
	{
		KalaWindowCore::ForceClose(
			"Initialization error",
			"Failed to attach an Input context to window '" + window->GetTitle() + "'!");

		return nullptr;
	}

	return window;
}