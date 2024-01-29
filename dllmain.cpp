#include "includes.hpp"
#include "gui.hpp"
#include "content.hpp"
#include "memory.hpp"
#include "version.hpp"
#include "console.hpp"
#include "hooks.hpp"
#include "crash-handler.hpp"

extern "C" __declspec(dllexport) int __stdcall stub() { return 0; }

void Main()
{
    Console::Init();
    Console::WriteF("GDH Build %s\n", GDH_VERSION);
    Console::WriteF("Build date: %s %s\n", GDH_BUILD_DATE, GDH_BUILD_TIME);

    if (!Content::load("GDH/content.json")) {
        Console::WriteLn("GDH initialization canceled due to an invalid content file");
        return;
    }
    
    if (MH_Initialize() != MH_OK)
    {
        Console::WriteLn("Failed to initialize MinHook");
        return;
    }
    else
        Console::WriteLn("MinHook successfully initialized");

    
    crashHandler::init();
    ImGuiHook::setRenderFunction(gui::RenderMain);
    ImGuiHook::setUnloadFunction(gui::Unload);
    ImGuiHook::setKeyPressHandler(gui::KeyPress);
    ImGuiHook::Load([](void *target, void *hook, void **trampoline)
                            { MH_CreateHook(target, hook, trampoline); });

    hooks::init();
    
    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
        Console::WriteLn("Failed to enable hooks");
    else
        Console::WriteLn("Successfully enabled all hooks");
    Console::WriteLn("Note: If you have the Steam version, it's likely that the Steam API will write logs to my console.\nI didn't intend for this!");
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        Main();
    }
    else if (fdwReason == DLL_PROCESS_DETACH) {
        Content::save();
    }
    return true;
}
