#ifndef BASE_LUA_BINDER
#define BASE_LUA_BINDER

#include <memory>
#include <string>
#include <sol.hpp>
#include "./macros.hpp"

namespace APICore
{
    template <class ParentClassType, class T, StringLiteral BinderTag, class... ExtraData>
    struct LuaBinder
    {
        using type_definition = TypeLookup<T>;
        static void bind(sol::state &state, void *userType, void *memberPtr, std::string key)
        {
            int i = 0;
        }
    };

    template <class ParentClassType, class T, class... ExtraData>
        requires requires {
                     requires std::is_class<ParentClassType>::value;
                     requires !std::is_pointer<T>::value;
                 }
    struct LuaBinder<ParentClassType, T, "MEMBER", ExtraData...>
    {
        using type_definition = TypeLookup<T>;



        static void bind(sol::state &state, sol::usertype<ParentClassType> *userType, T ParentClassType::*memberPtr, std::string key)
        {
            DependOnType(state,T);
            sol::usertype<ParentClassType> &parentType = *userType;
            parentType[key] = memberPtr;
        }
        static void bind(sol::state &state, sol::usertype<ParentClassType> *userType, T *ptr, std::string key)
        {
            DependOnType(state,T);
            sol::usertype<ParentClassType> &parentType = *userType;
            T &value = *ptr;
            parentType[key] = sol::var(std::ref(value));
        }
    };

}

#include "./class-binder.hpp"
#include "./reference-binder.hpp"

#endif