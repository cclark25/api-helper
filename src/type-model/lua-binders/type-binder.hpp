#ifndef BASE_LUA_BINDER
#define BASE_LUA_BINDER

#include <memory>
#include <string>
#include <sol.hpp>
#include "./macros.hpp"

namespace APICore
{

    template <class T>
    struct LuaBinderGenerator
    {
        static sol::usertype<T> generateType(sol::state &state)
        {
            return state.new_usertype<T>(
                TypeLookup<T>::registeredType::name,
                sol::constructors<T()>());
        }
    };

    template <class T>
    struct LuaBinder
    {
        using type_definition = TypeLookup<T>;
        static std::map<sol::state *, sol::usertype<T>> usertypeDeclarations;

        // template<class ParentClassType>
        // static void bind(sol::state &state, sol::usertype<ParentClassType> *userType, void *memberPtr, std::string key)
        // {
        // }

        static sol::usertype<T> declareType(sol::state &state)
        {
            if (!LuaBinder<T>::usertypeDeclarations.contains(&state))
            {
                sol::usertype<T> uType = LuaBinderGenerator<T>::generateType(state);
                usertypeDeclarations[&state] = uType;
            }
            return LuaBinder<T>::usertypeDeclarations.at(&state);
            // return LuaBinderGenerator<T>::generateType(state);
        }
    };

    template <class T>
    std::map<sol::state *, sol::usertype<T>> LuaBinder<T>::usertypeDeclarations;

    // template <class T>
    //     requires requires {
    //                  //  requires std::is_class<ParentClassType>::value;
    //                  requires !std::is_pointer<T>::value;
    //              }
    // struct LuaBinder<T>
    // {
    //     using type_definition = TypeLookup<T>;

    //     template <class ParentClassType>
    //         requires std::is_class<ParentClassType>::value
    //     static void bind(sol::state &state, sol::usertype<ParentClassType> *userType, T ParentClassType::*memberPtr, std::string key)
    //     {
    //         DependOnType(state, T);
    //         sol::usertype<ParentClassType> &parentType = *userType;
    //         parentType[key] = memberPtr;
    //     }

    //     template <class ParentClassType>
    //         requires std::is_class<ParentClassType>::value
    //     static void bind(sol::state &state, sol::usertype<ParentClassType> *userType, T *ptr, std::string key)
    //     {
    //         DependOnType(state, T);
    //         sol::usertype<ParentClassType> &parentType = *userType;
    //         T &value = *ptr;
    //         parentType[key] = sol::var(std::ref(value));
    //     }
    // };

}

#include "./reference-binder.hpp"
#include "./class-binder.hpp"

#endif