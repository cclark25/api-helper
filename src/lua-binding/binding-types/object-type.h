#ifndef LUA_BINDING_OBJECT_TYPE
#define LUA_BINDING_OBJECT_TYPE

#include <sol/sol.hpp>
#include "../../data-wrappers/data-wrapper.hpp"
#include "../../data-wrappers/object-wrapper.hpp"
#include "./create-binding-object.h"
#include "./basic-binders.h"
#include "./lua-object-to-data-wrapper.h"

namespace APILua
{
    using namespace APICore;
    class SolTableWrapper : public DataWrapperSub<DataPrimitive::object>
    {
    protected:
        sol::table tableData;

    public:
        SolTableWrapper(sol::table input) ;

        virtual Data<DataPrimitive::object> get();
    };

    template <>
    sol::table createBindingObject<DataPrimitive::object>(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::object>> wrapper);
}

#endif