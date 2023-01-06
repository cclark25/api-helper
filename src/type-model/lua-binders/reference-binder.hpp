#ifndef LUA_REFERENCE_BINDER
#define LUA_REFERENCE_BINDER

#include <memory>
#include <string>
#include <sol.hpp>
#include "./concepts.hpp"

namespace APICore
{
    template<ReferenceForLua T>
    struct dereference {
        using type = std::remove_reference_t<decltype(*T())>;
    };

    template <ReferenceForLua T>
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

    template <ReferenceForLua T>
    struct LuaBinder<T>
    {
        static sol::usertype<T> *declareType(sol::state &state)
        {
            LuaBinder<typename dereference<T>::type>::declareType(state);
            return nullptr;
        };
    };
    // template <class T>
    //     requires requires(T tval) {
    //                  {
    //                      *tval
    //                  };
    //              }
    // struct LuaBinder<T>
    // {
    //     using F = std::remove_pointer_t<T>;

    //     template<class ParentClassType>
    //     static void bind(sol::state &state, sol::usertype<ParentClassType> *userType, T ParentClassType::*memberPtr, std::string key)
    //     {
    //         DependOnType(state, F);

    //         // if constexpr (requires(F val) { {*val} ; }){
    //         //     F __val = nullptr;
    //         //     LuaBinder<void, std::remove_reference_t<decltype(*(__val))>, "ANY">::bind(state, nullptr);

    //         // }
    //         sol::usertype<ParentClassType> &parentType = *userType;

    //         parentType[key] = sol::property(
    //             [memberPtr](ParentClassType & parent) -> auto & {
    //                 auto &val = (*(parent.*memberPtr));
    //                 return val;
    //             },
    //             [memberPtr](ParentClassType *parent, F &val) -> void
    //             {
    //                 (*(parent->*memberPtr)) = val;
    //                 return;
    //             });
    //     }

    //     template<class ParentClassType>
    //     static void bind(sol::state &state, sol::usertype<ParentClassType> *userType, T *ptr, std::string key)
    //     {
    //         DependOnType(state, F);
    //         sol::usertype<ParentClassType> &parentType = *userType;
    //         T &value = *ptr;
    //         // parentType[key] = sol::var(std::ref(value));
    //     }
    // };

}

#endif