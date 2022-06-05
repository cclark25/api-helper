#ifndef LUA_BINDING_ARRAY_TYPE
#define LUA_BINDING_ARRAY_TYPE
#include "sol/sol.hpp"
#include "./create-binding-object.h"

namespace APILua {
    template <>
    sol::table createBindingObject<DataPrimitive::array>(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::array>> wrapper);
}

#endif