#pragma once

#include <iostream>

class ConsoleWindow
{
public:
    ConsoleWindow()
    {
        AllocConsole();

        const auto menu_handle = GetSystemMenu(GetConsoleWindow(), FALSE);
        DeleteMenu(menu_handle, SC_CLOSE, MF_BYCOMMAND);

        freopen_s(&output, "CONOUT$", "w", stdout);
    }
    ~ConsoleWindow() { fclose(output); }

private:
    FILE* output;
};

inline void enableDebug()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    static ConsoleWindow cw;
}