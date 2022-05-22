#ifndef LUA_WRAPPERS_BINDING
#define LUA_WRAPPERS_BINDING
#include "../data-wrappers/data-wrapper.hpp"
#include "../data-wrappers/int-wrapper.hpp"
#include <sol/sol.hpp>
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
    void BindBasicWrapper(sol::table luaState,
                          KeyType key,
                          std::shared_ptr<DataWrapper> wrapper);

    class TableDataWrapper : public DataWrapper
    {
    public:
        std::shared_ptr<ObjectWrapper> src;
        sol::state_view state;
        TableDataWrapper(sol::state_view state, std::shared_ptr<ObjectWrapper> src) : state(state), src(src)
        {
        }
        virtual bool canGet() { return true; };
        virtual bool canSet() { return true; };
        virtual DataPrimitive getDataType() { return DataPrimitive::object; };
        virtual Data get()
        {
            auto table = std::shared_ptr<sol::table>(new sol::table(this->state, sol::new_table()));
            auto data = src->get();
            auto fields = CastSharedPtr(ObjectMap, data);
            for (auto field : *fields)
            {
                BindWrapper<std::string>(*table, field.first, field.second);
            }
            return table;
        };
        virtual void set(Data data) { throw "Not Implemented!"; };
    };

    const std::string TEMPORARY_FIELD_NAME = "_temporaryField";

    sol::table BindDataWrapperLua(sol::state_view state, std::shared_ptr<DataWrapper> wrapper)
    {
        sol::table dataWrapperTable(state, sol::new_table());
        if (wrapper->canGet())
        {
#define Getter(type, wrapper) dataWrapperTable["get"] = [wrapper]() \
{                                                                   \
    auto data = wrapper->get();                                     \
    return *CastSharedPtr(type, data);                           \
}

            switch (wrapper->getDataType())
            {
            case DataPrimitive::null:
                break;
            case DataPrimitive::int32:
                Getter(int32_t, wrapper);
                break;
            case DataPrimitive::string:
                Getter(std::string, wrapper);
                break;
            case DataPrimitive::object:
                Getter(sol::table, wrapper);
                break;
            case DataPrimitive::array:
                break;
            case DataPrimitive::function:
                break;
            case DataPrimitive::classType:
                break;
            }

#undef Getter
        }

        if (wrapper->canSet())
        {
#define Setter(type, wrapper) dataWrapperTable["set"] = [wrapper](sol::table self, type value) { \
    wrapper->set(std::shared_ptr<type>(new type(value)));                                        \
}

            switch (wrapper->getDataType())
            {
            case DataPrimitive::null:
                break;
            case DataPrimitive::int32:
                Setter(int32_t, wrapper);
                break;
            case DataPrimitive::string:
                Setter(std::string, wrapper);
                break;
            case DataPrimitive::object:
                Setter(sol::table, wrapper);
                break;
            case DataPrimitive::array:
                break;
            case DataPrimitive::function:
                break;
            case DataPrimitive::classType:
                break;
            }

#undef Setter
        }

        return dataWrapperTable;
    }

    template <typename KeyType, typename InternalType>
    void BindBasicWrapper(sol::table luaState,
                          KeyType key,
                          std::shared_ptr<DataWrapper> wrapper)
    {
        auto state = sol::state_view(luaState.lua_state());
        auto wrapperTable = BindDataWrapperLua(state, wrapper);
        auto binder = sol::state_view(luaState.lua_state())["____bindingHelpers"]["bindWrapper"];
        binder(nullptr, key, wrapperTable, luaState);
        return;
    }

    // template <typename KeyType, typename InternalType>
    // void BindBasicWrapper(
    //     sol::table luaState,
    //     KeyType key,
    //     std::shared_ptr<DataWrapper> wrapper,
    //     std::function<InternalType()> getter = nullptr,
    //     std::function<void(sol::table self, InternalType value)> setter = nullptr)
    // {
    //     if (getter == nullptr)
    //     {
    //         getter = [wrapper]()
    //         {
    //             auto data = wrapper->get();
    //             return *(CastSharedPtr(InternalType, data));
    //         };
    //     }
    //     if (setter == nullptr)
    //     {
    //         setter = [wrapper](sol::table self, InternalType value)
    //         {
    //             wrapper->set(std::shared_ptr<InternalType>(new InternalType(value)));
    //         };
    //     }

    //     sol::table getterSetterObject(luaState.lua_state(), sol::new_table());
    //     auto setDescriptor = sol::state_view(luaState.lua_state())["____lualib"]["__TS__SetDescriptor"];
    //     auto x = setDescriptor.valid();
    //     if (wrapper->canGet())
    //     {
    //         getterSetterObject.set("get", getter);
    //     }
    //     if (wrapper->canSet())
    //     {
    //         getterSetterObject.set("set", setter);
    //     }

    //     setDescriptor(luaState, key, getterSetterObject, false);
    // }

    template <typename KeyType>
    void BindObjectWrapper(sol::table luaState,
                           KeyType key,
                           std::shared_ptr<ObjectWrapper> wrapper)
    {
        BindBasicWrapper<KeyType, sol::table>(
            luaState,
            key,
            std::shared_ptr<TableDataWrapper>(
                new TableDataWrapper(
                    sol::state_view(luaState.lua_state()),
                    wrapper)));
    }

    template <typename KeyType>
    void BindArrayWrapper(sol::table luaState,
                          KeyType key,
                          std::shared_ptr<ArrayWrapper> wrapper)
    {
        BindBasicWrapper<KeyType, sol::table>(
            luaState,
            key,
            wrapper,
            [wrapper, luaState]()
            {
                sol::table table(luaState.lua_state(), sol::new_table());
                auto data = wrapper->get();
                auto createGenerator = sol::state_view(luaState.lua_state())["____lualib"]["__TS__Generator"];
                auto generator = (sol::function)createGenerator([wrapper, luaState]()
                                                                {
                    auto data = wrapper->get();
                    auto elements = CastSharedPtr(ArrayVector, data);
                    auto yield = sol::state_view(luaState.lua_state())["coroutine"]["yield"];

                    sol::table tempTable(luaState.lua_state(), sol::new_table());
                    for(auto element : *elements){
                        // BindWrapper(tempTable, TEMPORARY_FIELD_NAME, element);
                        // yield(tempTable[TEMPORARY_FIELD_NAME]);
                        // yield("123");
                    } });
                auto state = sol::state_view(luaState.lua_state());

                auto symbol = ((state["____lualib"]["Symbol"]["iterator"]));
                // auto symbol = std::string(state.do_string("\
                //     return tostring(require('lualib_bundle').Symbol.iterator);\
                // "));
                // auto z = symbol.get_type();

                table[symbol] = [generator]()
                {
                    return generator();
                };

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
        case DataPrimitive::array:
            // BindArrayWrapper<KeyType>(luaState, key, CastSharedPtr(ArrayWrapper, wrapper));
            // auto bindingHelpers = sol::state_view(luaState.lua_state)['____bindingHelpers'];

            break;

        default:
            throw sol::error("Unsupported type.");
            break;
        }
    }

    int LoadFile(lua_State *L)
    {
        std::string path = "binding-helpers/" + sol::stack::get<std::string>(L) + ".lua";

        auto i = luaL_loadfilex(L, path.c_str(), "text");
        if (i == 0)
        {
            return 1;
        }
        else
        {
            return i;
        }
    }
    void BindAPI(std::string apiName, sol::state &luaState, std::map<std::string, std::shared_ptr<DataWrapper>> source)
    {
        luaState.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table, sol::lib::math, sol::lib::string);
        luaState.add_package_loader(LoadFile);

        luaState.script("____lualib = require('lualib_bundle');");
        luaState.script("____bindingHelpers = require('binding-helpers');");

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