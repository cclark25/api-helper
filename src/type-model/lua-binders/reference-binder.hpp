#ifndef LUA_REFERENCE_BINDER
#define LUA_REFERENCE_BINDER

#include <memory>
#include <string>
#include <sol.hpp>

namespace APICore
{
    template <class ParentClassType, class T, StringLiteral BinderTag, class... ExtraData>
        requires requires {
                     requires std::is_pointer<T>::value;
                 }
    struct LuaBinder<ParentClassType, T, BinderTag, ExtraData...>
    {
        using F = std::remove_pointer_t<T>;

        static void bind(sol::state &state, sol::usertype<ParentClassType> *userType, T ParentClassType::*memberPtr, std::string key)
        {
            DependOnType(state, F);
            sol::usertype<ParentClassType> &parentType = *userType;

            parentType[key] = sol::property(
                [memberPtr](ParentClassType & parent) -> auto & {
                    auto &val = (*(parent.*memberPtr));
                    return val;
                },
                [memberPtr](ParentClassType & parent, F& val) -> void {
                    (*(parent.*memberPtr)) = val;
                })
                ;
        }
        static void bind(sol::state &state, sol::usertype<ParentClassType> *userType, T *ptr, std::string key)
        {
            DependOnType(state, F);
            sol::usertype<ParentClassType> &parentType = *userType;
            T &value = *ptr;
            // parentType[key] = sol::var(std::ref(value));
        }
    };

}

#include "./class-binder.hpp"

#endif