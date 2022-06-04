
#include <sol/sol.hpp>
#include "../../data-wrappers/data-wrapper.hpp"
#include "../../data-wrappers/object-wrapper.hpp"
#include "./object-type.h"
#include "./lua-object-to-data-wrapper.h"
#include "./create-binding-object.h"

namespace APILua
{

    SolTableWrapper::SolTableWrapper(sol::table input) : tableData(input) {}

    Data<DataPrimitive::object> SolTableWrapper::get()
    {
        Data<DataPrimitive::object> objectMap(new ObjectMap());
        for (auto f : this->tableData)
        {
            std::string key = f.first.as<std::string>();
            sol::object value = f.second;
            auto wrappedValue = luaObjectToDataWrapper(value);
            auto x = *CastSharedPtr(StringWrapper,wrappedValue)->get();
            objectMap->insert_or_assign(key, wrappedValue);
        }
        return objectMap;
    };

    template <>
    sol::table createBindingObject<DataPrimitive::object>(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::object>> wrapper)
    {
        sol::table table = sol::table(state, sol::new_table());
        if (wrapper->canGet())
        {
            table["get"] = [wrapper, &state]()
            {
                sol::table newTable(state, sol::new_table());
                auto fields = wrapper->get();

                for (auto field : *fields)
                {
                    auto bindingObject = createBindingObject(state, CastSharedPtr(DataWrapperSub<DataPrimitive::unknown>, field.second));
                    APILua::bindToLua<std::string>(field.first, state, bindingObject, &newTable);
                }

                return newTable;
            };
        }
        if (wrapper->canSet())
        {
            table["set"] = [wrapper](sol::table self, data_primitive_to_lua_type<DataPrimitive::object> data)
            {
                auto newWrapper = SolTableWrapper(data);

                wrapper->set(newWrapper.get());

                return;
            };
        }

        return table;
    }

}
