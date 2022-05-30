#ifndef LUA_BINDING_TYPES
#define LUA_BINDING_TYPES
#include <sol/sol.hpp>
#include <type_traits>
#include <string>
#include <functional>

#include "../../data-wrappers/data-wrapper.hpp"
#include "../../data-wrappers/object-wrapper.hpp"
#include "../lua-loader.hpp"

namespace APILua
{
    using namespace std;
    using namespace APICore;

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

    template <DataPrimitive D>
    struct _data_primitive_to_lua_type
    {
        using type = typename _data_primitive_to_type<D>::type;
    };
#define CreateDataType(Primitive, Type)           \
    template <>                                   \
    struct _data_primitive_to_lua_type<Primitive> \
    {                                             \
        using type = Type;                        \
    }
    CreateDataType(DataPrimitive::array, sol::table);
    CreateDataType(DataPrimitive::object, sol::table);
#undef CreateDataType

    template <DataPrimitive D>
    using data_primitive_to_lua_type = typename _data_primitive_to_lua_type<D>::type;

    template <DataPrimitive D>
    sol::table createBindingObject(sol::state &state, std::shared_ptr<DataWrapperSub<D>> wrapper);

    // Placeholder to prevent the generic templating from expanding for object wrappers. Actual implementation in a different file.
    template <>
    sol::table createBindingObject(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::object>> wrapper);

    template <>
    sol::table createBindingObject(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::null>> wrapper)
    {
        return sol::table(state, sol::new_table());
    }
    template <>
    sol::table createBindingObject(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::function>> wrapper)
    {
        return sol::table(state, sol::new_table());
    }
    template <>
    sol::table createBindingObject(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::array>> wrapper)
    {
        return sol::table(state, sol::new_table());
    }
    template <>
    sol::table createBindingObject(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::classType>> wrapper)
    {
        return sol::table(state, sol::new_table());
    }

    template <DataPrimitive D>
    sol::table createBindingObject(sol::state &state, std::shared_ptr<DataWrapperSub<D>> wrapper)
    {
        sol::table table = sol::table(state, sol::new_table());
        if (wrapper->canGet())
        {
            table["get"] = [wrapper]()
            {
                auto ptr = wrapper->get();
                return *ptr;
            };
        }
        if (wrapper->canSet())
        {
            table["set"] = [wrapper](sol::table self, data_primitive_to_lua_type<D> data)
            {
                wrapper->set(Data<D>(new data_primitive_to_lua_type<D>(data)));
            };
        }

        return table;
    }

    template <>
    sol::table createBindingObject<DataPrimitive::unknown>(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::unknown>> wrapper)
    {
#define CaseData(Primitive)                                                                                                            \
    case DataPrimitive::Primitive:                                                                                                     \
        return createBindingObject<DataPrimitive::Primitive>(state, CastSharedPtr(DataWrapperSub<DataPrimitive::Primitive>, wrapper)); \
        break

        switch (wrapper->getDataType())
        {
            CaseData(string);
            CaseData(int32);
            CaseData(object);
            CaseData(null);
            CaseData(function);
            CaseData(classType);

        default:
            throw sol::error("Could not bind type.");
            break;
        }

#undef CaseData
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

#include "./object-type.hpp"

#endif