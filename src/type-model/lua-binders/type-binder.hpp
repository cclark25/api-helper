#ifndef BASE_LUA_BINDER
#define BASE_LUA_BINDER

#include <memory>
#include <string>
#include <sol.hpp>

namespace APICore {
    template<class T, StringLiteral BinderTag = "ANY", class... ExtraData>
    struct LuaBinder;

    template<class... ExtraData>
    struct LuaBinder<std::string, "ANY", ExtraData...> {
        static void bind(sol::state & state){
        }
    };
    
    template<class... ExtraData>
    struct LuaBinder<int, "ANY", ExtraData...> {
        static void bind(sol::state & state){
        }
    };

    template<class... ExtraData>
    struct LuaBinder<void, "ANY", ExtraData...> {
        static void bind(sol::state & state){
        }
    };

    template<class... ExtraData>
    struct LuaBinder<double, "ANY", ExtraData...> {
        static void bind(sol::state & state){
        }
    };
}

#include "./class-binder.hpp"
#include "./function-binder.hpp"

#endif