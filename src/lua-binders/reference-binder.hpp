#ifndef LUA_REFERENCE_BINDER
#define LUA_REFERENCE_BINDER

#include <memory>
#include <string>
#include <sol.hpp>
#include "../concepts.hpp"

namespace APICore
{

    template <ReferenceTypeConcept T>
    struct LuaBinderGenerator<T>
    {
        static sol::usertype<T> *generateType(sol::state &state)
        {
            sol::usertype<T> *newClassType = new sol::usertype<T>();
            (*newClassType) = state.new_usertype<void>(
                TypeLookup<T>::registeredType::name);
            return newClassType;
        };
    };

    template <ReferenceTypeConcept T>
    struct LuaBinder<T>
    {
        static sol::usertype<T> *declareType(sol::state &state)
        {
            LuaBinder<typename dereference<T>::type>::declareType(state);
            return nullptr;
        };
    };
}

#endif