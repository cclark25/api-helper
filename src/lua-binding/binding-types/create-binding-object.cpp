#include "./create-binding-object.h"

#include "../../data-primitive.hpp"
#include "../../data-wrappers/data-wrapper.hpp"
#include "./type-templates.h"
#include "./basic-binders.h"

namespace APILua
{
    using namespace APICore;
    template <DataPrimitive D>
    sol::table createBindingObject(sol::state &state, std::shared_ptr<DataWrapperSub<D>> wrapper);

    // Placeholder to prevent the generic templating from expanding for object wrappers. Actual implementation in a different file.
    template <>
    sol::table createBindingObject(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::object>> wrapper);

    template <>
    sol::table createBindingObject(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::null>> wrapper)
    {
        auto bindingObject = sol::table(state, sol::new_table());
        bindingObject["get"] = [](){};
        return bindingObject;
    }
    template <>
    sol::table createBindingObject(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::function>> wrapper);
    template <>
    sol::table createBindingObject(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::array>> wrapper);
    template <>
    sol::table createBindingObject(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::classType>> wrapper);
    // template <>
    // sol::table createBindingObject(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::classInstance>> wrapper);

    template <DataPrimitive D>
    sol::table createBindingObject(sol::state &state, std::shared_ptr<DataWrapperSub<D>> wrapper)
    {
        sol::table table = sol::table(state, sol::new_table());
        if (wrapper->canGet())
        {
            table["get"] = [wrapper]()
            {
                auto ptr = wrapper->get();
                return *ptr;
            };
        }
        if (wrapper->canSet())
        {
            table["set"] = [wrapper](sol::table self, data_primitive_to_lua_type<D> data)
            {
                wrapper->set(Data<D>(new data_primitive_to_lua_type<D>(data)));
            };
        }

        return table;
    }

    template <>
    sol::table createBindingObject<DataPrimitive::unknown>(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::unknown>> wrapper)
    {
#define CaseData(Primitive)                                                                                                            \
    case DataPrimitive::Primitive:                                                                                                     \
        return createBindingObject<DataPrimitive::Primitive>(state, CastSharedPtr(DataWrapperSub<DataPrimitive::Primitive>, wrapper)); \
        break
        
        if (wrapper == nullptr)
        {
            return createBindingObject<DataPrimitive::null>(state, CastSharedPtr(DataWrapperSub<DataPrimitive::null>, wrapper));
        }
        switch (wrapper->getDataType())
        {
            CaseData(string);
            CaseData(int32);
            CaseData(object);
            CaseData(null);
            CaseData(function);
            CaseData(array);
            CaseData(classType);
            // CaseData(classInstance);

        default:
            throw sol::error("Could not bind type.");
            break;
        }

#undef CaseData
    }

}
