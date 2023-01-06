#ifndef BASE_LUA_BINDER
#define BASE_LUA_BINDER

#include <memory>
#include <string>
#include <sol.hpp>
#include "./macros.hpp"
#include "./concepts.hpp"

namespace APICore
{

    template <class T>
    struct LuaBinderGenerator
    {
        static sol::usertype<T> *generateType(sol::state &state)
        {
            sol::usertype<T> *utype = new sol::usertype<T>();
            (*utype) = state.new_usertype<T>(
                TypeLookup<T>::registeredType::name,
                sol::constructors<T()>());

            return utype;
        }
    };

    template <class T>
    struct LuaBinder
    {
        using type_definition = TypeLookup<T>;
        static std::map<sol::state *, sol::usertype<T> *> usertypeDeclarations;

        static sol::usertype<T> *declareType(sol::state &state);
    };
    template <class T>
        requires requires { requires !ReferenceForLua<T>; }
    sol::usertype<T> *LuaBinder<T>::declareType(sol::state &state)
    {
        if (!LuaBinder<T>::usertypeDeclarations.contains(&state))
        {
            sol::usertype<T> *uType = LuaBinderGenerator<T>::generateType(state);
            usertypeDeclarations[&state] = uType;
        }
        return LuaBinder<T>::usertypeDeclarations.at(&state);
    };

    template <class T>
    std::map<sol::state *, sol::usertype<T> *> LuaBinder<T>::usertypeDeclarations;

    template <ReferenceForLua T>
    struct LuaBinder<T>;

}

#include "./reference-binder.hpp"
#include "./class-binder.hpp"

#endif