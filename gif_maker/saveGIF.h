#ifndef SAVEGIF
#define SAVEGIF

#include "linkedList.h"
#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <tchar.h>
#include <commctrl.h>


#ifndef UNICODE
#define UNICODE
#endif 

enum PATH_STATUS { UNVAILD = 0, VAILD_PATH };
enum PARMETERS {FILE_PATH = 1, DURATION_NUM, FRAME_NAME};

#define WCHAR_T_BYTES 2 // this type is commom in gui, it like char but takes 2 bytes insted of 1 like char
#define GET_PROJECT_PATH 7
#define VAILD_FILE 1

LRESULT CALLBACK beforeMain(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int WINAPI beforeWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
char* returnProjectPath(void);
enum PATH_STATUS checkPath(char* path, char* option);
void saveInfoGif(FrameNode* head, FILE* file);
enum PATH_STATUS readInfoGif(FrameNode** head, FILE* file);


#endif
