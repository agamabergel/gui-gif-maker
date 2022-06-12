/*********************************
* Class: MAGSHIMIM C2			 *
* AGAM ABERGEL     			     *
**********************************/
/*********************************
* Class: MAGSHIMIM C2			 *
* AGAM ABERGEL     			     *
**********************************/

#pragma comment(lib,"user32.lib") 
#define CV_IGNORE_DEBUG_BUILD_GUARD
#define _CRT_SECURE_NO_WARNINGS
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <stdio.h>
#include <opencv2/core/core_c.h>
#include <opencv2/highgui/highgui_c.h>
#include <windows.h>
#include <stdlib.h>
#include <tchar.h>

#include "linkedList.h"
#include "view.h"
#include "saveGIF.h"

#define NUM_BUTTONS 8
#define MAX_LEN_NAME 35
#define WCHAR_T_BYTES 2
#define MAX_CHARS 1024

enum INPUTS { PATH = 1, NAME, DURATION, INDEX };
enum OPTIONS { EXIT = 0, ADD = 10, REMOVE, CHANGE_INDEX, CHANGE_DURATION, CHANGE_ALL_DURATION, LIST, PLAY, SAVE, FIND }; // starting from 10 for not duplicites

const char g_szClassName[] = "Agam-GIFMaker";
FrameNode* head;
HBITMAP hBitmap;

/*
* This function creates all the text fileds in the perent window
* input: perent window, name, x&y value, width & height
* output: none
*/
void createTextWindow(HWND hwnd, wchar_t* name, int x, int y, int width, int height)
{
    CreateWindowW(_T("Static"),
        name,
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        x, y, width, height,
        hwnd, (HMENU)1, NULL, NULL);
}

/*
* This function loads the image
* input: imgPath
* output: none
*/
void LoadMyImage(wchar_t* imgPath) {

    hBitmap = LoadImageW(NULL, imgPath, IMAGE_BITMAP,
        150, 150, LR_LOADFROMFILE);
}

/*
* This function will convert all paths of the frames to a bmp type images using python script
* input: the window parent - hwnd
* output: none
*/
void loadFramesInWindow(HWND hwnd)
{
    FrameNode* copyHead = head;
    HWND hsti;
    const char* startCommand = "python convertTypesImage.py";
    char* tmpCommand = NULL;
    char* tmpPath = NULL;
    int xValue = 5, yValue = 5, countDownLines = 0, endLineVal = 855;

    while (copyHead)
    {
        wchar_t pathInWchar_t[MAX_CHARS];
        wchar_t durationAndName[MAX_CHARS];
        char durationAndNameStr[MAX_CHARS] = { '\0' };

        sprintf(durationAndNameStr, "Name: %s\nDuration: %d", copyHead->frame->name, copyHead->frame->duration);

        tmpPath = copyHead->frame->path;

        char* dupPath = (char*)malloc(strlen(tmpPath) * sizeof(char) + 1); // creating a duplicte of the path because the tmpPath has the same memory of the paths in head
        strncpy(dupPath, tmpPath, strlen(tmpPath) + 1);

        char* command = (char*)malloc(sizeof(char) * (strlen(startCommand) + strlen(tmpPath) + 5));
        sprintf(command, "%s \"%s\"", startCommand, tmpPath);

        system(command); // creating copy of the image but in bmp type using python script

        dupPath[strlen(dupPath) - 3] = NULL; // cutting the end of the new bmp images
        strcat(dupPath, "bmp"); // creatring new path for the bmp image

        swprintf(pathInWchar_t, MAX_CHARS, L"%hs", dupPath); // converting path into wchar_t type
        swprintf(durationAndName, MAX_CHARS, L"%hs", durationAndNameStr); // this time comverting frame name&duration to wchar_t

        LoadMyImage(pathInWchar_t);
        hsti = CreateWindowW(_T("Static"), _T(""),
            WS_CHILD | WS_VISIBLE | SS_BITMAP,
            xValue, yValue, 30, 300, hwnd, (HMENU)1, NULL, NULL);

        SendMessage(hsti, STM_SETIMAGE,
            (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap);

        createTextWindow(hwnd, durationAndName, xValue, yValue + 130, 150, 30);

        free(dupPath);
        free(command);

        if (xValue < endLineVal + 170) // when the row did not reach her end
        {
            xValue += 170;
        }

        else if (xValue == endLineVal || xValue == endLineVal + 170)
        {
            countDownLines++;
            xValue = 5; // returning back to the start of the row
            yValue = 175 * countDownLines; // going a row under as a dependency of xValue (when a end of a row reached)
        }
        
        copyHead = copyHead->next;
    }
}

/*
* This window shows the frames and lists them
* input: the window perent, the status msg, wParam, lParam
* output: LRESULT CALLBACK - The callee cleans the stack, so the compiler makes vararg functions
*/
LRESULT CALLBACK WindowListFrames(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:

            loadFramesInWindow(hwnd);
            break;

        case WM_DESTROY:

            //DeleteObject(hBitmap);
            PostQuitMessage(0);
            break;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

/*
* This function configs the list frames window
* input: hInstance, hPrevInstance, lpCmdLine, nCmdShow - all init as NULL usually
* output: int status
*/
int WINAPI WinListFrames(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd = NULL;
    MSG Msg;

    // initlizing windows struct info
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WindowListFrames; // function pointer
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // gray background
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "load-project";
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) // checking if an error accureced
    {
        MessageBox(NULL, _T("Window Registration Failed!"), _T("Error!"),
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Creating the Perent Window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "load-project",
        _T("List frames"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1900, 800,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        MessageBox(NULL, _T("Window Creation Failed!"), _T("Error!"),
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);


    while (GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    UnregisterClassW(wc.lpszClassName, hInstance); // freeing the memory value of the name so we can load this window everytime

    return Msg.wParam;
}

/*
* This function create the input boxes
* input: perent window, pointer to y axis value, one option for the inpit fileds
* output: HWNND struct
*/
static HWND createInputBoxes(HWND hwnd, int* y, int option)
{
    HWND tmp = CreateWindowW(_T("Edit"), _T(""),
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        250, *y, 800, 80,
        hwnd, (HMENU)option, NULL, NULL);

    *y += 120;
    return tmp;
}



/*
* This function controls the inputs from the user in the window, the MAIN FUNCTION!!!
* input: main window, msg, wParam, lParam 
* output: status
*/
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HWND inputPathBox = NULL, inputNameBox = NULL, inputDurationBox = NULL, inputIndexBox = NULL;
    HWND hwndButton = NULL;
    int y = 50;

    switch (msg)
    {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_CREATE:
        createTextWindow(hwnd, _T("\n  Instructions : \n\n\tOption 1 : path && name && duration\n\tOption 2 : name\n\tOption 3 : name && index\n\tOption 4 : name && duration\n\tOption 5 : duration\n\tOption 8 : path"), 250, 400, 300, 180);
        createTextWindow(hwnd, _T("  Path"), 250, 25, 80, 30);
        createTextWindow(hwnd, _T("  Name"), 250, 145, 80, 30);
        createTextWindow(hwnd, _T("  Index"), 750, 380, 80, 30);
        createTextWindow(hwnd, _T("  Duration"), 250, 265, 80, 30);

        inputIndexBox = CreateWindowW(_T("Edit"), _T(""),
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            750, 400, 300, 50,
            hwnd, (HMENU)INDEX, NULL, NULL);
        inputPathBox = createInputBoxes(hwnd, &y, PATH);
        inputNameBox = createInputBoxes(hwnd, &y, NAME);
        inputDurationBox = createInputBoxes(hwnd, &y, DURATION);

        break;

    case WM_COMMAND:
    {
        int lenPath = GetWindowTextLengthW(inputPathBox) + 1, lenName = GetWindowTextLengthW(inputNameBox) + 1, lenDuration = GetWindowTextLengthW(inputDurationBox) + 1, lenIndex = GetWindowTextLengthW(inputIndexBox) + 1;
        int dupLenPath = lenPath * WCHAR_T_BYTES, dupLenName = lenName * WCHAR_T_BYTES, dupLenDuration = lenDuration * WCHAR_T_BYTES, dupLenIndex = lenIndex * WCHAR_T_BYTES;
        wchar_t* path = (wchar_t*)(malloc(sizeof(wchar_t) * lenPath)), * name = (wchar_t*)(malloc(sizeof(wchar_t) * lenName)), * duration = (wchar_t*)(malloc(sizeof(wchar_t) * lenDuration)), * index = (wchar_t*)(malloc(sizeof(wchar_t) * lenIndex));
        char* dupPath = (char*)(malloc(sizeof(char) * dupLenPath)), * dupName = (char*)(malloc(sizeof(char) * dupLenName)), * dupDuration = (char*)(malloc(sizeof(char) * dupLenDuration)), * dupIndex = (char*)(malloc(sizeof(char) * dupLenIndex));

        /*
        * ^^^^ Explanation on the mallocing and all the stuff above ^^^^
        * The type of the input text that we get in gui is a wchar_t type -> that takes of a 2 bytes in memory.
        * Because of that, I can't use any function that takes char types and overall nothing from <string.h>.
        * So, I needed to convert the arrays from wchar_t into char type.
        *
        * Those are my steps:
        * Step 1: getting the length out of the input text (wchar_t type) and get a new length of the wchar_t array multilplied by 2.
        * Step 2: creating wchar_t type arrays for the input first, before converting. (used malloc)
        * Step 3: creatring the dupilcates of those array but in char type.
        * Step 4: Using wcstombs function for putting the converted text into a char* buffer with the length i multiplied by 2
        */
        GetWindowTextW(inputPathBox, path, lenPath);
        GetWindowTextW(inputNameBox, name, lenName);
        GetWindowTextW(inputDurationBox, duration, lenDuration);
        GetWindowTextW(inputIndexBox, index, lenIndex);

        wcstombs(dupDuration, duration, dupLenDuration);
        wcstombs(dupName, name, dupLenName);
        wcstombs(dupPath, path, dupLenPath);
        wcstombs(dupIndex, index, dupLenIndex);

        if (LOWORD(wParam) == EXIT)
        {
            PostQuitMessage(0);
        }
        else if (LOWORD(wParam) == ADD)
        {
            int flag = 0;
            if (checkPath(dupPath, "rb") == VAILD_PATH)
            {
                flag = VAILD_PATH;
                if (findNameFrame(head, dupName) == FOUND)
                {
                    MessageBox(NULL, _T("This name frame already have been taken!"), _T("Error"), MB_ICONWARNING);
                    return;
                }
                if (atoi(dupDuration) <= 0)
                {
                    MessageBox(NULL, _T("Unvaild duration!"), _T("Error"), MB_ICONWARNING);
                    return;
                }
            }
            if (flag == VAILD_PATH)
            {
                FrameNode* newNode = createFrame(dupName, (unsigned int)atoi(dupDuration), dupPath);
                addPhotoNode(&head, newNode);
                MessageBox(NULL, _T("Frame was added!"), _T("Succses!"), NULL);
            }

        }
        else if (LOWORD(wParam) == REMOVE)
        {
            if (removeFrameNode(&head, dupName) == NOT_FOUND)
            {
                MessageBox(NULL, _T("This Name does not exitst!"), _T("Error"), MB_ICONWARNING);
            }
            else
            {
                MessageBox(NULL, _T("Frame have been removed!"), _T("Succses!"), MB_OK);
            }
        }
        else if (LOWORD(wParam) == CHANGE_INDEX)
        {
            if (atoi(dupIndex) <= 0)
            {
                MessageBox(NULL, _T("Index not in range of the number frames!"), _T("Error"), MB_ICONWARNING);
                return;
            }
            if (changeIndexFrame(&head, dupName, atoi(dupIndex)) == NOT_FOUND)
            {
                MessageBox(NULL, _T("This Name or Index does not exitst!"), _T("Error"), MB_ICONWARNING);
            }
            else
            {
                MessageBox(NULL, _T("Frame have been changed it index!"), _T("Succses!"), MB_OK);
            }
        }
        else if (LOWORD(wParam) == CHANGE_DURATION)
        {
            if (atoi(dupDuration) <= 0)
            {
                MessageBox(NULL, _T("Unvaild duration!"), _T("Error"), MB_ICONWARNING);
                return;
            }
            if (findNameFrame(head, dupName) == NOT_FOUND)
            {
                MessageBox(NULL, _T("This Name does not exitst!"), _T("Error"), MB_ICONWARNING);
            }
            else
            {
                changeDurationFrame(&head, dupName, (unsigned int)atoi(dupDuration));
                MessageBox(NULL, _T("Duration frame have been changed!"), _T("Succses!"), MB_OK);
            }
        }
        else if (LOWORD(wParam) == CHANGE_ALL_DURATION)
        {
            if (atoi(dupDuration) <= 0)
            {
                MessageBox(NULL, _T("Unvaild duration!"), _T("Error"), MB_ICONWARNING);
                return;
            }
            changeAllDurationFrames(&head, (unsigned int)atoi(dupDuration));
            MessageBox(NULL, _T("All duration frames have been changed!"), _T("Succses!"), MB_OK);
        }
        else if (LOWORD(wParam) == LIST)
        {
            if (head == NULL)
                MessageBox(NULL, _T("No frames have been added yet!"), _T("Error"), MB_ICONWARNING);
            else
                WinListFrames(GetModuleHandle(NULL), NULL, NULL, SW_SHOW);
        }
        else if (LOWORD(wParam) == PLAY)
        {
            if (head == NULL)
                MessageBox(NULL, _T("No frames have been added yet!"), _T("Error"), MB_ICONWARNING);
            else
                play(head);
        }
        else if (LOWORD(wParam) == SAVE)
        {
            beforeWinMain(GetModuleHandle(NULL), NULL, NULL, SW_SHOW);

            char* loadProjectPath = returnProjectPath();
            if (checkPath(loadProjectPath, "wb") == VAILD_PATH)
            {
                FILE* file = fopen(loadProjectPath, "wb");
                saveInfoGif(head, file);
                MessageBox(NULL, _T("Frames have been saved succsesfuly!"), _T("Succses!"), MB_OK);
                fclose(file);
            }
            free(loadProjectPath);
        }

        free(path);
        free(duration);
        free(name);
        free(dupName);
        free(dupDuration);
        free(dupPath);
        break;
    }
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

/*
* This function creates a button for the program
* input: name of the button, the parent window pointer, x&y axis values, width of the button and flag for the button.
* output: none
*/
void createButton(wchar_t* buttonName, HWND* perent, int x, int* y, int width, int flag, int* i)
{
    HWND hwndButton = CreateWindowW(
        _T("BUTTON"),  // Predefined class; Unicode assumed 
        buttonName,      // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        x,         // x position 
        *y,         // y position 
        width,        // Button width
        50,        // Button height
        *perent,     // Parent window
        (HMENU)flag,       // No menu.
        (HINSTANCE)GetWindowLongPtr(*perent, GWLP_HINSTANCE),
        NULL);      // Pointer not needed.
    *y += 70;
    *i += 1;
}
/*
* This function will add all the buttons to the parent window with their actions
* input: buttonNames, perent window
* output: none
*/
void addButtons(wchar_t buttonNames[][MAX_LEN_NAME], HWND* perent)
{
    int i = 0, xValue = 20, yValue = 30, width = 185;
    createButton(buttonNames[i], perent, xValue, &yValue, width, EXIT, &i); // each time incresing i and yValue inside the function
    createButton(buttonNames[i], perent, xValue, &yValue, width, ADD, &i);
    createButton(buttonNames[i], perent, xValue, &yValue, width, REMOVE, &i);
    createButton(buttonNames[i], perent, xValue, &yValue, width, CHANGE_INDEX, &i);
    createButton(buttonNames[i], perent, xValue, &yValue, width, CHANGE_DURATION, &i);
    createButton(buttonNames[i], perent, xValue, &yValue, width, CHANGE_ALL_DURATION, &i);
    createButton(buttonNames[i], perent, xValue, &yValue, width, LIST, &i);
    createButton(buttonNames[i], perent, xValue, &yValue, width, SAVE, &i);

    width = 250;
    xValue = 1000;
    yValue = 500;
    createButton(_T("PLAY!"), perent, xValue, &yValue, width, PLAY, &i);
}

/*
* This function configs the main window
* input: hInstance, hPrevInstance, lpCmdLine, nCmdShow - all init as NULL usually
* output: int status
*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd = NULL;
    MSG Msg;

    wchar_t nameButtons[NUM_BUTTONS][MAX_LEN_NAME] = { _T("Exit"),
                                                    _T("Add New Frame"),
                                                    _T("Remove Frame"),
                                                    _T("Change frame Index"),
                                                    _T("Change frame duration"),
                                                    _T("Change all frames duration"),
                                                    _T("List frames"),
                                                    _T("Save project!") };

    // initlizing windows struct info
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc; // function pointer
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, _T("Window Registration Failed!"), _T("Error!"),
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Creating the Perent Window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        _T("THE GIF MAKER MADE BY AGAM ABERGEL THE QUEEN!!!"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1300, 700,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        MessageBox(NULL, _T("Window Creation Failed!"), _T("Error!"),
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    addButtons(nameButtons, &hwnd);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);


    while (GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}



int main()
{
    FILE* file = NULL;
    char* loadProjectPath = NULL;

    system("python -m pip install Pillow"); // installing pillow for python

    if (MessageBox(NULL, _T("Would you like to load saved project?"), _T("Welcome to Magshimim Movie Maker!!"), MB_YESNO) == IDYES)
    {
        beforeWinMain(GetModuleHandle(NULL), NULL, NULL, SW_SHOW); // checking user for loading path
        loadProjectPath = returnProjectPath();
        if (checkPath(loadProjectPath, "rb") == VAILD_PATH)
        {
            file = fopen(loadProjectPath, "rb");
            if (readInfoGif(&head, file) == UNVAILD)
            {
                head = NULL;
            }
            WinMain(GetModuleHandle(NULL), NULL, NULL, SW_SHOW);

            fclose(file);
        }
        else
        {
            WinMain(GetModuleHandle(NULL), NULL, NULL, SW_SHOW);
        }
        free(loadProjectPath);
    }
    else
    {
        WinMain(GetModuleHandle(NULL), NULL, NULL, SW_SHOW);
    }
    freeLinkedList(head);
}
