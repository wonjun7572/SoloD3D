#pragma once

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/ImGuizmo.h"
#include "ImGui/ImGuiFileDialog.h"

#pragma warning (disable : 4005)
#pragma warning (disable : 4251)

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

#include <math.h>

#include <Effects11//d3dx11effect.h>

#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK/ScreenGrab.h>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/VertexTypes.h>
#include <DirectXTK/Effects.h>

#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>
#include <wincodec.h>

#include <DirectXTK/SimpleMath.h>

#include <d3dcompiler.h>
#include <TypeInfo.h>
#include <DirectXCollision.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <thread>
#include <process.h>

#include <map>
#include <list>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
using namespace std;

// 크래쉬 확인용
#include <cassert>

#include "Engine_Typedef.h"
#include "Engine_Macro.h"
#include "Engine_Function.h"
#include "Engine_Struct.h"

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG

using namespace Engine;