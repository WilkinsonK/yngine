#pragma once
#include <string>

namespace Engine::UI::Sdl {
    typedef enum UiSdl_State_Code {
        USC_NOT_READY,
        USC_READY,
        USC_FAILED,
        USC_SHUTDOWN,
    } UiSdl_State_Code;

    class UiSdl_State {
        public:
        UiSdl_State(void);
        const UiSdl_State_Code Code(void);
        bool IsReady(void);
        bool IsFailed(void);
        bool IsShutdown(void);
        void SetReady(void);
        void SetFailed(const char*);
        void SetShutdown(void);
        const char *Status(void);
        private:
        UiSdl_State_Code code;
        std::string      status;

        UiSdl_State(const UiSdl_State_Code, const char *);
        inline bool IsState(const UiSdl_State_Code);
        inline void SetState(const UiSdl_State_Code, const char *);
    };
}
