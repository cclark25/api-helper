#ifndef LUA_OBJECT_TO_DATA_WRAPPER
#define LUA_OBJECT_TO_DATA_WRAPPER

#include <sol/sol.hpp>
#include "../../data-primitive.hpp"
#include "../../data-wrappers/data-wrapper.hpp"
#include "./object-type.h"

namespace APILua
{
    using namespace APICore;

    std::shared_ptr<DataWrapper> luaObjectToDataWrapper(sol::object luaData);
}

#endif