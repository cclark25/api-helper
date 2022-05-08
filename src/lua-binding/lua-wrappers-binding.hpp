#ifndef LUA_WRAPPERS_BINDING
#define LUA_WRAPPERS_BINDING
#include "../data-wrappers/data-wrapper.hpp"
#include "../data-wrappers/int-wrapper.hpp"
#include <sol/sol.hpp>
#include <lualib_bundle.h>
#include <map>
#include <functional>

namespace APILua
{
    using namespace APICore;
    using namespace sol;


    template <typename KeyType>
    void BindWrapper(
        sol::table luaState,
        KeyType key,
        std::shared_ptr<DataWrapper> wrapper);

    template <typename KeyType, typename InternalType>
    void BindBasicWrapper(
        sol::table luaState,
        KeyType key,
        std::shared_ptr<DataWrapper> wrapper,
        std::function<InternalType()> getter = nullptr,
        std::function<void(sol::table self, InternalType value)> setter = nullptr)
    {
        if (getter == nullptr)
        {
            getter = [wrapper]()
            {
                auto data = wrapper->get();
                return *(CastSharedPtr(InternalType, data));
            };
        }
        if (setter == nullptr)
        {
            setter = [wrapper](sol::table self, InternalType value)
            {
                wrapper->set(std::shared_ptr<InternalType>(new InternalType(value)));
            };
        }

        sol::table getterSetterObject(luaState.lua_state(), sol::new_table());
        auto setDescriptor = sol::state_view(luaState.lua_state())["____lualib"]["__TS__SetDescriptor"];

        if (wrapper->canGet())
        {
            getterSetterObject.set("get", getter);
        }
        if (wrapper->canSet())
        {
            getterSetterObject.set("set", setter);
        }

        setDescriptor(luaState, key, getterSetterObject, false);
    }

    template <typename KeyType>
    void BindObjectWrapper(sol::table luaState,
                           KeyType key,
                           std::shared_ptr<ObjectWrapper> wrapper)
    {
        BindBasicWrapper<KeyType, sol::table>(
            luaState,
            key,
            wrapper,
            [wrapper, luaState]()
            {
                sol::table table(luaState.lua_state(), sol::new_table());
                auto data = wrapper->get();
                auto fields = CastSharedPtr(ObjectMap, data);
                for (auto &field : *fields)
                {
                    BindWrapper<std::string>(table, field.first, field.second);
                }
                return table;
            });
    }

    /**
     * KeyType is string or int
     */
    template <typename KeyType>
    void BindWrapper(
        sol::table luaState,
        KeyType key,
        std::shared_ptr<DataWrapper> wrapper)
    {
        switch (wrapper->getDataType())
        {
        case DataPrimitive::string:
            BindBasicWrapper<KeyType, std::string>(luaState, key, wrapper);
            break;
        case DataPrimitive::int32:
            BindBasicWrapper<KeyType, int32_t>(luaState, key, wrapper);
            break;
        case DataPrimitive::null:
            break;
        case DataPrimitive::object:
            BindObjectWrapper<KeyType>(luaState, key, CastSharedPtr(ObjectWrapper, wrapper));
            break;
            // case DataPrimitive::string:
            //     break;

        default:
            throw sol::error("Unsupported type.");
            break;
        }
    }

    void BindAPI(std::string apiName, sol::state &luaState, std::map<std::string, std::shared_ptr<DataWrapper>> source)
    {
        luaState.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table, sol::lib::math, sol::lib::string);
        luaState.require_script("lualib_bundle", ((char *)lualib_bundle), true);
        luaState.script("____lualib = require('lualib_bundle');");

        luaState[apiName] = sol::new_table();
        auto table = (sol::table)(luaState[apiName]);

        for (auto definition : source)
        {
            // bindAPI(table, definition);
            BindWrapper<std::string>(table, definition.first, definition.second);
        }
    }
}

#endif