#pragma once
#include <memory>

#include <SDL3/SDL_render.h>

#include "EngineUI/ManagedInt.hpp"
#include "EngineUI_SDL/Globals.hpp"

namespace Engine::UI::Sdl {
    using namespace Engine::UI::Managed;

    class UiSdl_RendererI : public UiManagedObject {
        public:
        virtual ~UiSdl_RendererI(void) = default;
        virtual void Drop(void) override = 0;
        virtual void Drop(UiSdl_Globals) = 0;
        virtual UiSdl_RendererI& Init(void) override = 0;
        virtual UiSdl_RendererI& Init(UiSdl_Globals) = 0;
        virtual UiSdl_RendererI& Render(void) = 0;
    };
    typedef std::unique_ptr<UiSdl_RendererI> UiSdl_Renderer;

    class UiSdl3_Renderer : public UiSdl_RendererI {
        public:
        UiSdl3_Renderer(void);
        void Drop(void) override;
        void Drop(UiSdl_Globals) override;
        UiSdl3_Renderer& Init(void) override;
        UiSdl3_Renderer& Init(UiSdl_Globals) override;
        UiSdl3_Renderer& Render(void) override;
        private:
        SDL_Renderer *Renderer;
        bool CanInit(UiSdl_Globals);
        bool CanInit_GlobalReady(UiSdl_Globals);
        bool CanInit_WindowReady(UiSdl_Globals);
        bool CanInitOrDrop_GlobalsAvailable(UiSdl_Globals);
        UiSdl3_Renderer& Create_Renderer(UiSdl_Globals);
        UiSdl3_Renderer& Set_RenderVSync(void);
    };

    template <class R>
    UiSdl_Renderer MakeSdlRenderer(void);
}
