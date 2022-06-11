#ifndef LUA_BINDING_FUNCTION_TYPE
#define LUA_BINDING_FUNCTION_TYPE

#include <sol/sol.hpp>
#include "../../data-wrappers/data-wrapper.hpp"
#include "../../data-wrappers/object-wrapper.hpp"
#include "./create-binding-object.h"
#include "./basic-binders.h"
#include "./lua-object-to-data-wrapper.h"

namespace APILua
{
    using namespace APICore;

    template <>
    sol::table createBindingObject<DataPrimitive::function>(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::function>> wrapper);
}

#endif