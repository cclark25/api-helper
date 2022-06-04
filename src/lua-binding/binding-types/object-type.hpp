#ifndef LUA_BINDING_OBJECT_TYPE
#define LUA_BINDING_OBJECT_TYPE

#include <sol/sol.hpp>
#include "../../data-wrappers/data-wrapper.hpp"
#include "../../data-wrappers/object-wrapper.hpp"
#include "./create-binding-object.hpp"
#include "./basic-binders.hpp"

namespace APILua
{
    using namespace APICore;
    class SolTableWrapper : public DataWrapperSub<DataPrimitive::object>
    {
    protected:
        sol::table tableData;

    public:
        SolTableWrapper(sol::table input) : tableData(input) {}

        virtual Data<DataPrimitive::object> get()
        {
            Data<DataPrimitive::object> objectMap;
            for (auto f : this->tableData)
            {
                std::string key = f.first.as<std::string>();
                sol::object value = f.second;
                objectMap->insert_or_assign(key, std::shared_ptr<StringWrapper>(new StringContainerWrapper(key)));
                
            }
            return objectMap;
        };
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
                for (auto f : data)
                {
                    std::string key = f.first.as<std::string>();
                    sol::object value = f.second;
                    return;
                }
            };
        }

        return table;
    }

}

#endif