#pragma once

#include <cstring>
#include <exception>
#include <iostream>
#include <sstream>

#include "Traits.hpp"
namespace plugin::state {
    #ifndef __plugin_state_enumerated
    #define __plugin_state_enumerated
    /**
     * @brief Internal state code. Indicates the error
     * uniquely.
    */
    typedef unsigned long long __state;
    const __state __max_state = -1;
    const __state __min_state =  0;
    #define __statedef inline static
    __statedef bool __isstate(const __state lhs, const __state rhs) {
        return lhs == rhs;
    }
    __statedef bool __isstate_first(const __state s) {
        return __isstate(s, __min_state);
    }
    __statedef bool __isstate_last(const __state s) {
        return __isstate(s, __max_state);
    }
    #undef __statedef
    /**
     * @brief Internal state message. Indicates the error
     * as a human-readable value.
     */
    typedef const char *__text;
    /**
     * @brief Size of the internal text.
     */
    const std::size_t __text_s = sizeof(__text);
    /**
     * @brief The range of available states.
     */
    typedef unsigned int __slots;
    const __slots  __max_slots = sizeof(__state)*8;
    const __slots  __min_slots = 0;
    static __slots __iota_next = 0;
    /**
     * @brief Generates a new state code.
     * @warning The number of state codes is limited to the
     * capacity of the `__state` type. `state`s are used
     * like flags in a bitmap allowing for compound states.
     * @returns A new `__state` code value.
     */
    static __state __iota(void) {
        if (!__isstate_last(__iota_next)) return 0x1 << (__iota_next++);
        throw new std::runtime_error("max state types declared");
    }

    class State;
    static const State __make_registered_state(__text);
    static const State *__find_registered_state(__text);

    typedef struct State : public plugin::traits::Display {
        State(const __state c) {
            State::__init__(*this, c, NULL, 0);
        }
        State(const __state c, const __text t) {
            State::__init__(*this, c, NULL, 0);
            this->add_text(t);
        }
        State(const State& other) {
            State::__init__(*this, other.code, other.slots, other.slots_used);
        }

        bool operator==(const State& other) const {
            return State::__eq__(*this, other);
        }

        const State operator&(const State& other) const {
            return State::__land__(*this, other);
        }

        const State operator|(const State& other) const {
            return State::__lor__(*this, other);
        }

        friend const State *__find_registered_state(__text);
        private:
        __state code;
        __text  slots[__max_slots];
        __slots slots_used;

        void add_text(const __text t) {
            if (slots_used+1 > __max_slots)
                throw std::runtime_error("max state types concatenated");
            this->slots[slots_used++] = t;
        }

        void set_code(__state c) {
            code = c;
        }

        __slots slots_available(void) {
            return __max_slots - slots_used;
        }

        std::ostream& display(std::ostream& os) {
            os << "state<" << code << ">([";
            for (__slots i = 0; i < slots_used; i++) {
                os << "'" << slots[i] << "'";
                if (i+1 == slots_used) break;
                os << ", ";
            }
            return os << "])";
        }

        static void __init__(State& inst, __state code, const __text slots[__max_slots], __slots used) {
            inst.code = code;
            inst.slots_used = 0;

            memset(inst.slots, 0, __max_slots*__text_s);
            for (__slots i = 0; i < used; i++) {
                inst.add_text(slots[i]);
            }
        }

        static const bool  __eq__(const State& lhs, const State& rhs) {
            return (lhs.code & rhs.code) == rhs.code;
        }

        static const State __lor__(const State& lhs, const State& rhs) {
            if ((lhs & rhs).code != 0) return lhs;

            State result = State(lhs);
            if (result.slots_available() < rhs.slots_used)
                throw new std::runtime_error("slots available too limited for concatentation");
            // NOTE: does not check for duplicates beyond
            // this point.
            for (__slots i = 0; i < rhs.slots_used; i++) {
                result.add_text(rhs.slots[i]);
            }

            result.code |= rhs.code;
            return result;
        }

        static const State __land__(const State& lhs, const State& rhs) {
            State result = State(lhs);
            for (__slots i = 0; i < rhs.slots_used; i++) {
                if ((__find_registered_state(rhs.slots[i])->code & result.code) != 0) continue;
                if (result.slots_available() < rhs.slots_used-i)
                    throw new std::runtime_error("slots available too limited for concatentation");
                result.add_text(rhs.slots[i]);
            }
            result.code &= rhs.code;

            return result;
        }
    } state;

    static state *__state_registry[__max_slots];
    static bool __state_registry_init = false;

    inline static void __init_registry(void) {
        if (__state_registry_init) return;
        memset(__state_registry, 0, __max_slots*sizeof(state*));
        __state_registry_init = true;
    }

    inline static const state __make_registered_state(__text t) {
        __init_registry();
        state s = state(__iota(), t);
        __state_registry[__iota_next-1] = &s;
        return s;
    }

    static const state *__find_registered_state(__text t) {
        for (__slots i = 0; i < __iota_next; i++)
            if (__state_registry[i] != nullptr && std::strcmp(__state_registry[i]->slots[0], t) == 0)
                return __state_registry[i];
        return nullptr;
    }

    const state VOID_STATE = state(0, "void");
    #define __state(IDENT, ...) \
        const state IDENT = __make_registered_state(__VA_ARGS__);
    __state(AVAILABLE, "plugin module found")
    __state(DESTROYED, "plugin destroyed")
    __state(INSTALLED, "install successful")
    __state(STARTED, "installed plugin started")
    __state(STOPPED, "installed plugin stopped")
    #define __fail_inst(IDENT) INSTALL_FAIL_##IDENT
    #define __sfaili(IDENT, ...) __state(__fail_inst(IDENT), __VA_ARGS__)
    /** @brief Bad DLL/SO binary format. */
    __sfaili(MALFORMAT, "bad DLL/SO binary format")
    /** @brief Module callback missing. */
    __sfaili(NOCALLBACK, "module callback missing")
    /** @brief Module not installed. */
    __sfaili(NOINSTALL, "module not installed")
    /** @brief Module not loaded. */
    __sfaili(NOLOAD, "module not loaded")
    /** @brief Module file not found. */
    __sfaili(NOMODULE, "module file not found")
    /** @brief No object in module. */
    __sfaili(NOOBJECT, "no object in module")
    #undef __sfaili
    #undef __state
    state INSTALL_FAILURE =
          INSTALL_FAIL_MALFORMAT
        | INSTALL_FAIL_NOCALLBACK
        | INSTALL_FAIL_NOINSTALL
        | INSTALL_FAIL_NOLOAD
        | INSTALL_FAIL_NOMODULE
        | INSTALL_FAIL_NOOBJECT
    ;
    const state FAILURE = INSTALL_FAILURE;
    #undef __sfail_inst

    #endif // ifndef __plugin_state_enumerated
}
