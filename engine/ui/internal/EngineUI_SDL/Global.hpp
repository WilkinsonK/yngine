#pragma once
#include <SDL3/SDL_video.h>
#include <memory>

#include "EngineUI/ManagedInt.hpp"
#include "EngineUI_SDL/Globals.hpp"
#include "EngineUI_SDL/Renderer.hpp"

namespace Engine::UI::Sdl {
    using namespace Engine::UI::Managed;

    class UiSdl_Global : public UiManagedObject {
        public:
        UiSdl_Global(void);
        void Drop(void) override;
        UiSdl_Global& Drop_Global(void);
        UiSdl_Global& Drop_Renderer(void);
        UiSdl_Global& Drop_Window(void);
        float Display_GetContentScale(void);
        float Display_GetContentScale(SDL_DisplayID);
        UiSdl_Global& Init(void) override;
        UiSdl_Global& Init_Global(void);
        UiSdl_Global& Init_Renderer(void);
        UiSdl_Global& Init_Window(void);
        UiSdl_Global& Use_Globals(void);
        UiSdl_Global& Use_Globals(UiSdl_Globals);
        UiSdl_Global& Use_InitOpts(void);
        UiSdl_Global& Use_InitOpts(UiSdl_Global_InitOpts&);
        template <class R>
        UiSdl_Global& Use_Renderer(void);
        UiSdl_Global& Use_Renderer(UiSdl_Renderer);
        private:
        UiSdl_Globals Globals;
        UiSdl_Renderer Renderer;
        UiSdl_Global_InitOpts InitOpts;
    };
}
