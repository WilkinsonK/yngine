#pragma once
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <memory>

#include "EngineUI_SDL/State.hpp"

namespace Engine::UI::Sdl {
    #define __DeclareState(Ident) UiSdl_State Ident
    struct UiSdl_Global_States {
        __DeclareState(Global);
        __DeclareState(Renderer);
        __DeclareState(Window);
    };

    struct UiSdl_GlobalsI {
        SDL_Renderer       *Renderer;
        SDL_Window         *Window;
        UiSdl_Global_States States;
    };
    typedef std::shared_ptr<UiSdl_GlobalsI> UiSdl_Globals;

    struct UiSdl_Global_InitOpts_Window {
        SDL_WindowFlags Flags;
        const char      *Title;
        float           Scale;
        unsigned long   Height;
        unsigned long   Width;
    };

    struct UiSdl_Global_InitOpts {
        SDL_InitFlags                Flags;
        UiSdl_Global_InitOpts_Window Window;
    };

    UiSdl_Globals MakeGlobals(void);
}
