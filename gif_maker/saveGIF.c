#include "saveGIF.h"

char* dupPathProject = NULL;

LRESULT CALLBACK beforeMain(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    FrameNode* head = NULL;
    wchar_t path = NULL;
    static HWND hwndEdit = NULL;
    HWND hwndButton = NULL, hwndText = NULL;

    switch (msg)
    {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_CREATE:

            hwndText = CreateWindowW(_T("Static"), _T("Enter a file path in this text box:"), // creating text
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                200, 20, 330, 20,
                hwnd, (HBRUSH)(COLOR_WINDOW + 1), NULL, NULL);

            hwndEdit = CreateWindowW(_T("Edit"), NULL, // creating text box
                WS_CHILD | WS_VISIBLE | WS_BORDER,
                20, 50, 650, 50,
                hwnd, (HMENU)GET_PROJECT_PATH, NULL, NULL);

            hwndButton = CreateWindowW(_T("Button"), _T("ENTER"), // creating a button
                WS_VISIBLE | WS_CHILD, 
                200, 160, 270, 50,
                hwnd, (HMENU)GET_PROJECT_PATH, NULL, NULL);
            break;

        case WM_COMMAND:
            if (HIWORD(wParam) == BN_CLICKED)
            {
                int len = GetWindowTextLengthW(hwndEdit) + 1, dupLen = len * WCHAR_T_BYTES; // getting length of the text and convert it to char type
                char* path = (wchar_t*)(malloc(sizeof(wchar_t) * len)); // mallocing memory for the string in wchar_t bytes (thats the safest type in gui)

                dupPathProject = (char*)(malloc(sizeof(char) * dupLen)); // mallocing the string

                GetWindowTextW(hwndEdit, path, len);
                wcstombs(dupPathProject, path, dupLen); // convert wchar_t array into char array

                free(path);
                DestroyWindow(hwnd); // closing the window
            }
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;

}


int WINAPI beforeWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd = NULL;
    MSG Msg;

    // initlizing windows struct info
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = beforeMain; // function pointer
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
        _T("LOADING PREVIOUS PROJECT..."),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 700, 400,
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
* This function will return the users path of the previous project to load back to main
* input: none
* output: file path
*/
char* returnProjectPath(void)
{
    return dupPathProject;
}


/*
* This function checks if the project file to load exists.
* input: path, file pointer, option for reading\writing
* output: status of file path
*/
enum PATH_STATUS checkPath(char* path, char* option)
{
    FILE* file = NULL;

    if (path)
	    file = fopen(path, option);

    if (file == NULL)
    {
        MessageBox(NULL, _T("Path isn't vaild or does not exists."), _T("ERROR - [Returning back...]"), MB_ICONHAND);
        return UNVAILD;
    }
    fclose(file);
    return VAILD_PATH;
}

/*
* This function saves the linked list info to a file
* input: the head & file pointer
* output: none
*/
void saveInfoGif(FrameNode* head, FILE* file)
{
	FrameNode* copiedHead = head;
	while (copiedHead)
	{
		fprintf(file, "%s;%d;%s\n", copiedHead->frame->path, copiedHead->frame->duration, copiedHead->frame->name);
		copiedHead = copiedHead->next;
	}
}


/*
* This function extract the data from the info file
* input: file pointer
* output: the data
*/
char* extractData(FILE* file)
{
    int fileSize = 0;
    int i = 0, nullCount = 0;
    char* buffer = NULL;

    fseek(file, 0, SEEK_END);
    fileSize = ftell(file) + 1;
    rewind(file); // return file cursur to the start of the file

    buffer = malloc(sizeof(char) * fileSize + 1);
    if (buffer == NULL)
    {
        printf("not able to malloc!");
        exit(1);
    }

    fread(buffer, sizeof(char), fileSize, file);
    buffer[fileSize] = NULL; // to ignore unwanted symbols

    return buffer;
}



/*
* This function reads and makes a head with the previous project info
* input: head, file
* ouput: vaild path or not
*/
enum PATH_STATUS readInfoGif(FrameNode** head, FILE* file)
{
    char* path = NULL, * name = NULL, * duration = NULL, * data = extractData(file);
    const char splitLines = '\n', splitInfo = ';';
    int lenName = 0, lenPath = 0, lenDuration = 0;
    int parm = FILE_PATH, i = 0;

    for (i = 0; i < strlen(data); i++)
    {
        if (data[i] == splitInfo)
        {
            parm++;
        }
        if (parm == FILE_PATH && (data[i] != splitInfo && data[i] != splitLines)) // each time i realloc the strings +1 until i see a ;
        {
            path = (char*)realloc(path, sizeof(char) * lenPath + 1);
            path[lenPath] = data[i];
            lenPath++;
        }
        else if (parm == DURATION_NUM && (data[i] != splitInfo && data[i] != splitLines))
        {
            duration = (char*)realloc(duration, sizeof(char) * lenDuration + 1);
            duration[lenDuration] = data[i];
            lenDuration++;
        }
        else if (parm == FRAME_NAME && (data[i] != splitInfo && data[i] != splitLines))
        {
            name = (char*)realloc(name, sizeof(char) * lenName + 1);
            name[lenName] = data[i];
            lenName++;
        }
        if (data[i] == splitLines) // when returning into a new line -> init evreything
        {
            if (!path || !name || !duration) // if one of the parmeters arent vaild
            {
                MessageBox(NULL, _T("File is not in the right format!"), _T("Creating new project..."), MB_ICONEXCLAMATION);

                free(path);
                free(name);
                free(duration);
                free(data);

                return UNVAILD;
            }
            name[lenName] = '\0';
            path[lenPath] = '\0';
            duration[lenDuration] = '\0';

            FrameNode* newNode = createFrame(name, (unsigned int)atoi(duration), path); // crateing new frame with the paremters
            addPhotoNode(head, newNode);

            parm = FILE_PATH;  // restarting the vairbles
            lenName = 0;
            lenPath = 0;
            lenDuration = 0;

            path = (char*)realloc(path, sizeof(char));
            name = (char*)realloc(name, sizeof(char));
            duration = (char*)realloc(duration, sizeof(char));
        }

    }

    MessageBox(NULL, _T("Previous Project Loaded Succsesfuly!"), _T("SUCCSESS"), NULL);

    free(path);
    free(name);
    free(duration);
    free(data);

    return VAILD_PATH;
}