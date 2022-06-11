#ifndef API_LUA_TYPE_TEMPLATES
#define API_LUA_TYPE_TEMPLATES
#include <sol/sol.hpp>
#include <type_traits>
#include <string>
#include <functional>

#include "../../data-primitive.hpp"

namespace APILua
{
    using namespace std;
    using namespace APICore;

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
    CreateDataType(DataPrimitive::function, sol::function);
#undef CreateDataType

    template <DataPrimitive D>
    using data_primitive_to_lua_type = typename _data_primitive_to_lua_type<D>::type;

}

#endif