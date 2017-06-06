#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <iostream>
#include <thread>
#include <map>
#include <tchar.h>
#include <windows.h>
#include <gdiplus.h>

using namespace std;
using namespace Gdiplus;

#define WM_DRAWTEXT 10000000
#define WM_DRAWTYPE 10000001
#define WM_MOVE_TO  10000002

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

void threadFunction(HWND hwnd)
{
     while (TRUE)
     {
        Sleep(5000);
        WindowProcedure(hwnd, WM_MOVE_TO, rand() % 500, rand() % 500);
     }
}

void winPaintText(HWND hwnd, HDC hdc, int BG_COLOR)
// draw text in paint event
{
    for (int i=0; i<2; i++)
    {
        string strTest = "Overlay text: ";
        RECT a = {0,0+10*i,400,20+20*i};
        SetBkColor(hdc, RGB(0,0,45));
        SetTextColor(hdc,RGB(255,255,255));
        DrawText(hdc, strTest.c_str(), strTest.size(), &a, DT_LEFT);
    }
}

void winPaintIcons(HWND hwnd, HDC hdc, int BG_COLOR)
// draw icons of signature in paint event
{
    Graphics graphics(hdc);
    Image* image = Image::FromFile( L"image.png" , true);

        //buff + "D:\\icon.bmp" );
    cout << "The width of the image is " << image->GetWidth() << endl;
    cout << "The height of the image is " << image->GetHeight() << endl;

    Rect introBgRect(50, 50, image->GetWidth(), image->GetHeight());
    graphics.DrawImage(image, introBgRect, 0, 0, image->GetWidth(), image->GetHeight(), UnitPixel);
}

int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
        // console color
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 15);

    cout << "Create class for overlay: ";
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */

    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
    {
        cout << " ERROR" << endl << "Can't register the window class" << endl;
        return 0;
    }
    else
        cout << "OK" << endl;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           WS_EX_NOACTIVATE | WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT,/* Extended possibilites for variation */
           szClassName,                                                         /* Classname */
           _T("Code::Blocks Template Windows App"),                             /* Title Text */
           WS_POPUP,                                                            /* default window */
           100,                                                                 /* Windows decides the position */
           100,                                                                 /* where the window ends up on the screen */
           400,                                                                 /* The programs width */
           375,                                                                 /* and height in pixels */
           HWND_DESKTOP,                                                        /* The window is a child-window to desktop */
           NULL,                                                                /* No menu */
           hThisInstance,                                                       /* Program Instance handler */
           NULL                                                                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    DWORD WinVersion = (DWORD)(LOBYTE(LOWORD(GetVersion())));
    LONG BG_COLOR;
    if (WinVersion > 5)
        BG_COLOR = RGB(200,200,200);
    else
        BG_COLOR = RGB(214,211,206);

    ShowWindow (hwnd, nCmdShow);
    SetLayeredWindowAttributes(hwnd, BG_COLOR, 0, LWA_COLORKEY);

    cout << "Create parser thread: ";
    if (hwnd)
    {
        std::thread parserThread(threadFunction, hwnd);
        parserThread.detach();
        cout << "OK" << endl;
    }
    else
    {
        cout << "ERROR" << endl << "Can't create overlay. Application will close" << endl;
        return 0;
    }

    //WindowProcedure(hwnd, WM_PAINT, -5, NULL);
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);

        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    GdiplusShutdown(gdiplusToken);
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_DESTROY:
            PostQuitMessage(0);       /* send a WM_QUIT to the message queue */
            break;

        case WM_PAINT:
            {
                InvalidateRect(hwnd, NULL, TRUE);

                PAINTSTRUCT ps;
                HDC hdc;
                hdc=BeginPaint(hwnd,&ps);

                DWORD WinVersion = (DWORD)(LOBYTE(LOWORD(GetVersion())));
                LONG BG_COLOR;
                if (WinVersion > 5)
                    BG_COLOR = RGB(200,200,200);
                else
                    BG_COLOR = RGB(214,211,206);

                winPaintText(hwnd, hdc, BG_COLOR);
                winPaintIcons(hwnd, hdc, BG_COLOR);

                EndPaint(hwnd,&ps);
            }

            break;

        case WM_MOVE_TO:
            {
                MoveWindow(hwnd, wParam, lParam, 400, 375, TRUE);
            }

            break;

        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
