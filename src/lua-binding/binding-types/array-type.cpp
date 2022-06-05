#include "./array-type.h"
#include "./lua-object-to-data-wrapper.h"
#include "../lua-loader.hpp"

namespace APILua
{
    template <>
    sol::table createBindingObject<DataPrimitive::array>(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::array>> wrapper)
    {
        sol::table bindingTable = sol::table(state, sol::new_table());

        if (wrapper->canGet())
        {
            bindingTable["get"] = [wrapper, &state]()
            {
                auto binder = sol::state_view(state)["____bindingHelpers"]["bindArrayWrapper"];
                if (!binder.valid())
                {
                    state.add_package_loader(LoadFile);
                    state.script("____bindingHelpers = require('binding-helpers');");
                    binder = sol::state_view(state)["____bindingHelpers"]["bindArrayWrapper"];
                }

                sol::table arrayAccessorTable = sol::table(state, sol::new_table());
                arrayAccessorTable["getIndex"] = [wrapper, &state](size_t index)
                {
                    auto data = wrapper->getIndex(index);
                    return createBindingObject<DataPrimitive::unknown>(state, CastSharedPtr(UnknownWrapper, data))["get"]();
                };
                arrayAccessorTable["setIndex"] = [wrapper](size_t index, sol::object value)
                {
                    std::shared_ptr<DataWrapper> wrappedValue = luaObjectToDataWrapper(value);
                    wrapper->setIndex(index, wrappedValue);
                };
                arrayAccessorTable["push"] = [wrapper](sol::object value)
                {
                    std::shared_ptr<DataWrapper> wrappedValue = luaObjectToDataWrapper(value);
                    return wrapper->push(wrappedValue);
                };
                arrayAccessorTable["pop"] = [wrapper, &state]()
                {
                    auto data = wrapper->pop();
                    return createBindingObject<DataPrimitive::unknown>(state, CastSharedPtr(UnknownWrapper, data))["get"]();
                };
                arrayAccessorTable["insert"] = [wrapper](size_t index, sol::object value)
                {
                    std::shared_ptr<DataWrapper> wrappedValue = luaObjectToDataWrapper(value);
                    return wrapper->insert(index, wrappedValue);
                };

                arrayAccessorTable["length"] = [wrapper]()
                {
                    return wrapper->length();
                };

                auto boundResult = binder(nullptr, arrayAccessorTable);

                return boundResult;
            };
        }
        if (wrapper->canSet())
        {
            // bindingTable["set"] = [wrapper](sol::table self, data_primitive_to_lua_type<DataPrimitive::object> data) {

            // };
        }

        return bindingTable;
    }
}