#ifndef LUA_BASIC_BINDERS
#define LUA_BASIC_BINDERS
#include "./type-templates.hpp"
#include "../lua-loader.hpp"
#include "../../data-wrappers/data-wrapper.hpp"
#include "./create-binding-object.hpp"

namespace APILua {
    
    template <typename KeyType>
    void bindToLua(KeyType key, sol::state &state, sol::table getterSetter, sol::table *destination = nullptr)
    {
        auto binder = sol::state_view(state)["____bindingHelpers"]["bindWrapper"];
        if (!binder.valid())
        {
            state.add_package_loader(LoadFile);
            state.script("____bindingHelpers = require('binding-helpers');");
            binder = sol::state_view(state)["____bindingHelpers"]["bindWrapper"];
        }
        if (destination == nullptr)
        {
            binder(nullptr, key, getterSetter);
        }
        else
        {
            binder(nullptr, key, getterSetter, *destination);
        }
    }

    template <typename KeyType>
    void bind(KeyType key, sol::state &state, std::map<KeyType, std::shared_ptr<DataWrapper>> entries)
    {
        sol::table newTable(state, sol::new_table());
        for (std::pair<std::string, std::shared_ptr<DataWrapper>> field : entries)
        {
            bindToLua<std::string>(field.first, state, createBindingObject(state, CastSharedPtr(DataWrapperSub<DataPrimitive::unknown>, field.second)), &newTable);
        }

        state[key] = newTable;
    }
}

#endif