#include <gl/gl.h>
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <malloc.h>
#include "camera.h"
using namespace std;


float vert[] = {1, 1, 0,  1, -1, 0,  -1, -1, 0,  -1, 1, 0};
int n = 100; // размер пол€

void MovePlayer(){ // интерактив внутри сцены
    Camera_DirectionalMovement(GetKeyState('W')<0 ? 1 : (GetKeyState('S')< 0 ? -1 : 0),
                                GetKeyState('D')<0 ? 1 : (GetKeyState('A')< 0 ? -1 : 0),
                                0.1);
    Camera_MouseMovement(400,400,0.5);
}

void ShowWorld() { // отрисовка сцены
    glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, &vert);
        for (int i = -n/2; i < n/2; i++)
            for (int j = -n/2; j < n/2; j++) {
                glPushMatrix();
                    if ((i + j) % 2 == 0)
                        glColor3f(1, 1, 1); // белый сегмент
                    else
                        glColor3f(0, 0, 0.0); // черный сегмент

                    glTranslatef(i * 2, j * 2, 0);
                    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
                glPopMatrix();
            }
                glLineWidth(1);
    glBegin(GL_LINES);          // осевые направл€ющие
        glColor3d(1,0,0);
        glVertex3f(-1000,0,0);
        glVertex3f(1000,0,0);
        glColor3d(0,1,0);
        glVertex3f(0,-1000,0);
        glVertex3f(0,1000,0);
        glColor3d(0,0,1);
        glVertex3f(0,0,-1000);
        glVertex3f(0,0,1000);
    glEnd( );
    glDisableClientState(GL_VERTEX_ARRAY);
}
void windResize(HWND hwnd){
    RECT rct;
    GetClientRect(hwnd, &rct);
    int width = rct.right - rct.left;
    int height =rct.bottom - rct.top;
    glViewport(0, 0,width , height);
}
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          1280,
                          1280,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);
    glFrustum(-1,1, -1,1, 2,80);
    ShowCursor(false);

    /* program main loop */
    while (!bQuit) {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT) {
                bQuit = TRUE;
            }
            else {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else {

            glClearColor(0.2f, 0.3f, 0.1f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glPushMatrix();
                Camera_Apply();
                MovePlayer();
                windResize(hwnd);
                ShowWorld();
            glPopMatrix();
            SwapBuffers(hDC);
            Sleep (1);
        }
        if(GetKeyState(VK_ESCAPE)<0){
            bQuit = TRUE;
        }
    }

    DisableOpenGL(hwnd, hDC, hRC);

    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;
        case WM_DESTROY:
            return 0;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC) {
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    *hDC = GetDC(hwnd);

    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC) {
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}


