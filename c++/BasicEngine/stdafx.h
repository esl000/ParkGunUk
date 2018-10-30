// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <list>
#include <stack>
#include <queue>
#include <assert.h>
#include <math.h>


#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx10math.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

//#include <d3dx11.h>
//#include <xnamath.h>
//#include <dxerr.h>

//#pragma comment(lib, "d3d11.lib")
//#pragma comment(lib, "d3dx11.lib")
//#pragma comment(lib, "D3DCompiler.lib")
//#pragma comment(lib, "dxerr.lib")
//#pragma comment(lib, "dxgi.lib")
//#pragma comment(lib, "dxgiud.lib")

extern HWND g_hwnd;
extern void SetQuit(bool);
extern void SetResolution(RECT& rc);

#define HR(hresult) if(FAILED(hresult)){ assert(NULL && L"HRESULT INIT FAILED!"); }
#define SAFE_RELEASE(p) if(p){p->Release(); p = NULL;}
#define SAFE_DELETE(p) if(p){delete p; p = NULL;}
#define SAFE_DELETE_ARRAY(p) if(p){delete p; p = NULL;}
#define SINGLETONE(class_name)\
protected:\
class_name();\
~class_name();\
public:\
static class_name* GetInstance(){ static class_name instance; return &instance; }



#define PROPERTY(var_type, var_name, func_name)\
protected:\
var_type var_name;\
public:\
var_type Get##func_name(void){ return var_name; }\
void Set##func_name(var_type var){ var_name = var; }

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.


#include "cDeviceManager.h"