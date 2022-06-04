#ifndef LUA_OBJECT_TO_DATA_WRAPPER
#define LUA_OBJECT_TO_DATA_WRAPPER

#include <sol/sol.hpp>
#include "../../data-primitive.hpp"
#include "../../data-wrappers/data-wrapper.hpp"
#include "./object-type.hpp"

namespace APILua
{
    using namespace APICore;

    std::shared_ptr<DataWrapper> luaObjectToDataWrapper(sol::object luaData)
    {
#define CaseData(LuaType, Primitive)                               \
    case LuaType:                                                  \
        return std::shared_ptr<DataWrapper>(                       \
            new DataContainerWrapper<Primitive>(                   \
                luaData.as<data_primitive_to_type<Primitive>>())); \
        break;

        switch (luaData.get_type())
        {

            CaseData(sol::type::string, DataPrimitive::string);
            CaseData(sol::type::number, DataPrimitive::int32);
        case sol::type::nil:
            return nullptr;
        case sol::type::table:
            return std::shared_ptr<DataWrapper>(
                new SolTableWrapper(luaData.as<sol::table>()));
            break;
            // CaseData(sol::type::function);
            // CaseData(sol::type::classType);

        default:
            throw sol::error("Could not bind type.");
            break;
        }

#undef CaseData
    }

}

#endif