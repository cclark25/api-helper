#ifndef LUA_BINDING_CLASS_TYPE
#define LUA_BINDING_CLASS_TYPE

#include <sol/sol.hpp>
#include "../../data-wrappers/data-wrapper.hpp"
#include "./create-binding-object.h"
#include "./basic-binders.h"
#include "./lua-object-to-data-wrapper.h"

namespace APILua
{
    using namespace APICore;

    template <>
    sol::table createBindingObject<DataPrimitive::classType>(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::classType>> wrapper);

    template <>
    sol::table createBindingObject<DataPrimitive::classInstance>(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::classInstance>> wrapper);
}

#endif