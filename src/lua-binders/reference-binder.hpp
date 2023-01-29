#ifndef LUA_REFERENCE_BINDER
#define LUA_REFERENCE_BINDER

#include <memory>
#include <string>
#include <sol.hpp>
#include "../concepts.hpp"

namespace APICore
{


    template <ReferenceTypeConcept T>
    struct LuaBinder<T>
    {
        static sol::usertype<T> *declareType(sol::state &state)
        {
            LuaBinder<typename dereference<T>::type>::declareType(state);
            return nullptr;
        };
    };

    template <class ReturnType, class...Parameters>
    struct LuaBinder<ReturnType(*)(Parameters...)>
    {
        using T = ReturnType(*)(Parameters...);
        using InheritedFrom = TypeLookup<T>::inheritedFrom;

        using type_definition = TypeLookup<T>;
        static std::map<sol::state *, sol::usertype<T> *> usertypeDeclarations;

        static sol::usertype<T> *declareType(sol::state &state)
        {
            if (!LuaBinder<T>::usertypeDeclarations.contains(&state))
            {
                sol::usertype<T> *newType = new sol::usertype<T>;

                (*newType) = state.new_usertype<T>(
                    TypeLookup<T>::registeredType::name);

                usertypeDeclarations[&state] = newType;

                LuaBinderGenerator<T, InheritedFrom>::generateType(state, newType);
            }
            return LuaBinder<T>::usertypeDeclarations.at(&state);
        };
    };
   
    template <class ReturnType, class...Parameters> 
    std::map<sol::state *, sol::usertype<ReturnType(*)(Parameters...)> *> LuaBinder<ReturnType(*)(Parameters...)>::usertypeDeclarations;

    template <class ReturnType, class InheritedType, class...Parameters>
    struct LuaBinderGenerator<ReturnType(*)(Parameters...), InheritedType>
    {
        using T = ReturnType(*)(Parameters...);

        template<class ChildType = T>
        static void generateType(
            sol::state &state,
            sol::usertype<ChildType> *type)
        {
            // sol::usertype<T> *newClassType = new sol::usertype<T>();
            // (*newClassType) = state.new_usertype<void>(
            //     TypeLookup<T>::registeredType::name);
            
            return;
        };
    };
}

#endif