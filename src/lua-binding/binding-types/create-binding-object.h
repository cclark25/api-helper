#ifndef CREATE_BINDING_OBJECT
#define CREATE_BINDING_OBJECT

#include <sol/sol.hpp>
#include "../../data-primitive.hpp"
#include "../../data-wrappers/data-wrapper.hpp"
#include "./type-templates.h"
#include "./basic-binders.h"

namespace APILua {
    using namespace APICore;
    template <DataPrimitive D>
    sol::table createBindingObject(sol::state &state, std::shared_ptr<DataWrapperSub<D>> wrapper);

    // Placeholder to prevent the generic templating from expanding for object wrappers. Actual implementation in a different file.
    template <>
    sol::table createBindingObject(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::object>> wrapper);

    template <>
    sol::table createBindingObject(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::null>> wrapper);

    template <>
    sol::table createBindingObject(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::function>> wrapper);

    template <>
    sol::table createBindingObject(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::array>> wrapper);

    template <>
    sol::table createBindingObject(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::classType>> wrapper);

    template <DataPrimitive D>
    sol::table createBindingObject(sol::state &state, std::shared_ptr<DataWrapperSub<D>> wrapper);

    template <>
    sol::table createBindingObject<DataPrimitive::unknown>(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::unknown>> wrapper);
}

#include "./object-type.h"

#endif