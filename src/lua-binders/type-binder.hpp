#pragma once

#include <memory>
#include <string>
#include <map>
#include <sol.hpp>
#include "../concepts.hpp"


namespace APICore
{

    template <class T, class InheritedType>
    struct LuaBinderGenerator
    {
        template<class ChildType = T>
        static void generateType(sol::state &state, sol::usertype<ChildType> *type)
        {
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
        requires requires { 
            requires !ReferenceTypeConcept<T>; 
            requires !TemplatedDataConcept<T>;
    }
    sol::usertype<T> *LuaBinder<T>::declareType(sol::state &state)
    {
        if (!LuaBinder<T>::usertypeDeclarations.contains(&state))
        {
            using InheritedFrom = TypeLookup<T>::registeredType::inheritedFrom;
            sol::usertype<T> *newType = new sol::usertype<T>;

            (*newType) = state.new_usertype<T>(
                TypeLookup<T>::registeredType::name);

            usertypeDeclarations[&state] = newType;

            LuaBinderGenerator<T, InheritedFrom>::generateType(state, newType);
        }
        return LuaBinder<T>::usertypeDeclarations.at(&state);
    };

    template <class T>
    std::map<sol::state *, sol::usertype<T> *> LuaBinder<T>::usertypeDeclarations;

    template <ReferenceTypeConcept T>
    struct LuaBinder<T>;

}

#include "./reference-binder.hpp"
#include "./class-binder.hpp"
#include "./templated-data-binder.hpp"
#include "./function-binder.hpp"
