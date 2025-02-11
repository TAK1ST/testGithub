#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commctrl.h>

// Constants for controls
#define ID_LOGIN_BTN 1001
#define ID_REGISTER_BTN 1002
#define ID_USERNAME_EDIT 1003
#define ID_PASSWORD_EDIT 1004
#define ID_PROJECT_LIST 1005
#define ID_TASK_LIST 1006
#define ID_ADD_PROJECT_BTN 1007
#define ID_ADD_TASK_BTN 1008
#define ID_STATUS_UPDATE_BTN 1009

// Structure definitions
typedef struct {
    int id;
    char name[100];
    char description[256];
    char status[20];
    char assignedTo[50];
    char dueDate[20];
    int priority;
} Task;

typedef struct {
    int id;
    char name[100];
    char description[256];
    Task tasks[100];
    int taskCount;
} Project;

typedef struct {
    char username[50];
    char password[50];
    int isManager;  // 0 for Team Member, 1 for Project Manager
} User;

// Global variables
HWND hwndMain;
HWND hwndLogin;
HWND hwndProjectList;
HWND hwndTaskList;
User currentUser;
Project projects[100];
int projectCount = 0;

// Function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK LoginProc(HWND, UINT, WPARAM, LPARAM);
void CreateLoginWindow(HINSTANCE hInstance);
void CreateMainWindow(HINSTANCE hInstance);
void InitializeControls(HWND hwnd);
void HandleLogin(HWND hwnd);
void HandleRegistration(HWND hwnd);
void AddProject(HWND hwnd);
void AddTask(HWND hwnd, int projectId);
void UpdateTaskStatus(int taskId, const char* status);
void RefreshProjectList(void);
void RefreshTaskList(int projectId);

// Main window procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_CREATE:
            InitializeControls(hwnd);
            break;
            
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
                case ID_ADD_PROJECT_BTN:
                    if(currentUser.isManager) {
                        AddProject(hwnd);
                    }
                    break;
                    
                case ID_ADD_TASK_BTN:
                    if(currentUser.isManager) {
                        int sel = ListView_GetNextItem(hwndProjectList, -1, LVNI_SELECTED);
                        if(sel != -1) {
                            AddTask(hwnd, sel);
                        }
                    }
                    break;
                    
                case ID_STATUS_UPDATE_BTN:
                    if(!currentUser.isManager) {
                        int sel = ListView_GetNextItem(hwndTaskList, -1, LVNI_SELECTED);
                        if(sel != -1) {
                            UpdateTaskStatus(sel, "Completed");
                        }
                    }
                    break;
            }
            break;
            
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
            
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// Login window procedure
LRESULT CALLBACK LoginProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_CREATE:
            // Create login controls
            CreateWindow("STATIC", "Username:", WS_VISIBLE | WS_CHILD,
                        10, 10, 80, 25, hwnd, NULL, NULL, NULL);
            CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                        100, 10, 150, 25, hwnd, (HMENU)ID_USERNAME_EDIT, NULL, NULL);
            CreateWindow("STATIC", "Password:", WS_VISIBLE | WS_CHILD,
                        10, 40, 80, 25, hwnd, NULL, NULL, NULL);
            CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD,
                        100, 40, 150, 25, hwnd, (HMENU)ID_PASSWORD_EDIT, NULL, NULL);
            CreateWindow("BUTTON", "Login", WS_VISIBLE | WS_CHILD,
                        100, 70, 80, 25, hwnd, (HMENU)ID_LOGIN_BTN, NULL, NULL);
            CreateWindow("BUTTON", "Register", WS_VISIBLE | WS_CHILD,
                        190, 70, 80, 25, hwnd, (HMENU)ID_REGISTER_BTN, NULL, NULL);
            break;
            
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
                case ID_LOGIN_BTN:
                    HandleLogin(hwnd);
                    break;
                case ID_REGISTER_BTN:
                    HandleRegistration(hwnd);
                    break;
            }
            break;
            
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
            
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// Initialize main window controls
void InitializeControls(HWND hwnd) {
    // Create project list view
    hwndProjectList = CreateWindow(WC_LISTVIEW, "",
                                 WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT,
                                 10, 10, 300, 200, hwnd, (HMENU)ID_PROJECT_LIST,
                                 NULL, NULL);
    
    // Create task list view
    hwndTaskList = CreateWindow(WC_LISTVIEW, "",
                               WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT,
                               320, 10, 300, 200, hwnd, (HMENU)ID_TASK_LIST,
                               NULL, NULL);
    
    // Create buttons based on role
    if(currentUser.isManager) {
        CreateWindow("BUTTON", "Add Project", WS_VISIBLE | WS_CHILD,
                    10, 220, 100, 25, hwnd, (HMENU)ID_ADD_PROJECT_BTN, NULL, NULL);
        CreateWindow("BUTTON", "Add Task", WS_VISIBLE | WS_CHILD,
                    120, 220, 100, 25, hwnd, (HMENU)ID_ADD_TASK_BTN, NULL, NULL);
    } else {
        CreateWindow("BUTTON", "Update Status", WS_VISIBLE | WS_CHILD,
                    320, 220, 100, 25, hwnd, (HMENU)ID_STATUS_UPDATE_BTN, NULL, NULL);
    }
}

// WinMain function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc = {0};
    MSG msg;
    
    // Register the main window class
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszClassName = "TaskManagerClass";
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    
    if(!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
                  MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    
    // First create login window
    CreateLoginWindow(hInstance);
    
    // Message loop
    while(GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return msg.wParam;
}

// Helper functions implementation
void CreateLoginWindow(HINSTANCE hInstance) {
    WNDCLASSEX wc = {0};
    
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = LoginProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszClassName = "LoginClass";
    
    RegisterClassEx(&wc);
    
    hwndLogin = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "LoginClass",
        "Task Manager Login",
        WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 150,
        NULL, NULL, hInstance, NULL
    );
}

void CreateMainWindow(HINSTANCE hInstance) {
    hwndMain = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "TaskManagerClass",
        "Task Manager",
        WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL
    );
}