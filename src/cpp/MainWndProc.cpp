#include "MainWndProc.hpp"
#include <windowsx.h>
#include "AppEngine.hpp"

LRESULT CALLBACK MainWndProc(WindowHandle window_handle, UINT msg, WPARAM w_param, LPARAM l_param)
{
    switch (msg)
    {
        case WM_ACTIVATE: // window is activated or deactivated
            if (LOWORD(w_param) == WA_INACTIVE)
                APP.pause();
            else
                APP.unpause();
            return 0;

        case WM_SIZE: // window resized
            APP.width(LOWORD(l_param));
            APP.height(HIWORD(l_param));
            if (w_param == SIZE_MINIMIZED)
                APP.minimize();
            else if (w_param == SIZE_MAXIMIZED)
                APP.maximize();
            else if (w_param == SIZE_RESTORED)
                APP.restore();
            return 0;

        case WM_ENTERSIZEMOVE: // resize bars grabbed
            APP.resizingOn();
            return 0;

        case WM_EXITSIZEMOVE: // resize bars released
            APP.resizingOff();
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0); // exit code
            return 0;

        // The WM_MENUCHAR message is sent when a menu is active and the user presses a key that does not correspond to
        // any mnemonic or accelerator key.
        case WM_MENUCHAR:
            return MAKELRESULT(0, MNC_CLOSE); // Don't beep when we alt-enter.

        // Catch this message to prevent the window from becoming too small.
        case WM_GETMINMAXINFO:
            ((MINMAXINFO*)l_param)->ptMinTrackSize.x = 200;
            ((MINMAXINFO*)l_param)->ptMinTrackSize.y = 200;
            return 0;

        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN: APP.mouseDown(w_param, {GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param)}); return 0;
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP: APP.mouseUp(w_param, {GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param)}); return 0;
        case WM_MOUSEMOVE: APP.mouseMove(w_param, {GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param)}); return 0;
    }

    return DefWindowProc(window_handle, msg, w_param, l_param);
}
