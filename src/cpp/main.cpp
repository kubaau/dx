#include "AppEngine.hpp"
#include "AppTimer.hpp"
#include "DebugUtils.hpp"
#include "Window.hpp"

int WINAPI WinMain(_In_ HINSTANCE instance_handle, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
#if defined(DEBUG) | defined(_DEBUG)
    enableDebug();
#endif
    APP.run(make_unique<Window>(instance_handle, "DX", 1024, 768), make_unique<AppTimer>());
}
