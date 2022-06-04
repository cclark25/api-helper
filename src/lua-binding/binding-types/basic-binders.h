#ifndef LUA_BASIC_BINDERS
#define LUA_BASIC_BINDERS
#include <sol/sol.hpp>
#include <map>
#include "../../data-wrappers/data-wrapper.hpp"

namespace APILua
{

    template <typename KeyType>
    void bindToLua(KeyType key, sol::state &state, sol::table getterSetter, sol::table *destination = nullptr);

    template <typename KeyType>
    void bind(KeyType key, sol::state &state, std::map<KeyType, std::shared_ptr<APICore::DataWrapper>> entries);

}

#endif