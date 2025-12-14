#pragma once
#include <stdexcept>
#include <string>

#define _E_newi_DECL(ERR_T, ...) ERR_T(__VA_ARGS__)
#define _E_newi_IMPL(SUP_T, ...) : SUP_T(m) __VA_OPT__(, ) __VA_ARGS__ {}
#define ERR_Construct_METHOD_EXT(ERR_T, SUP_T, ...) _E_newi_DECL(ERR_T, ErrorMessage m) _E_newi_IMPL(SUP_T __VA_OPT__(, ) __VA_ARGS__)
#define ERR_Construct_METHOD(ERR_T) _E_newi_DECL(ERR_T, ErrorMessage)

#define _E_fm_IMPL_ext(ERR_T, SUP_T) { \
    return new ERR_T(SUP_T::GetMethodMessage(m).c_str()); \
}
#define _E_fm_IMPL(ERR_T) _E_fm_IMPL_ext(ERR_T, UiErrorInternal)
#define _E_fm_DECL_ext(ERR_T, ...) FromMethod(__VA_OPT__(__VA_ARGS__))
#define _E_fm_DECL(ERR_T) ERR_T * _E_fm_DECL_ext(ERR_T, MethodName)
#define ERR_FromMethod_METHOD(ERR_T)  _E_fm_DECL(ERR_T)
#define ERR_FromMethod_METHOD_EXT(ERR_T, SUP_T) _E_fm_DECL_ext(ERR_T, MethodName m) _E_fm_IMPL_ext(ERR_T, SUP_T)

#define ERR_GetMethodMessage_METHOD(...) std::string GetMethodMessage(MethodName)
#define ERR_GetMethodMessage_METHOD_EXT(...) GetMethodMessage(MethodName m)

namespace Engine::UI::Errors {
    typedef const char *ErrorMessage;
    typedef const char *MethodName;

    typedef struct FromOtherArgs {
        MethodName method_name;
    } FromOtherArgs;

    class UiError : public std::runtime_error {
        public:
            ERR_Construct_METHOD(UiError);
    };

    class UiInitError : public UiError {
        public:
        ERR_Construct_METHOD(UiInitError);
    };

    class UiInitSDLError : public UiInitError {
        public:
            ERR_Construct_METHOD(UiInitSDLError);
    };
}
