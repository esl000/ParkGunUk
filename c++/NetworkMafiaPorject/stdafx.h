// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

//#include <Windows.h>
//#include <stdio.h>
//#include <thread>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <set>
#include <map>
#include <chrono>
#include <mutex>
#include <ctime>

#pragma comment(lib, "ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>

#define SIZE_BUF 1024


extern std::mutex g_MainMutex;



// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
