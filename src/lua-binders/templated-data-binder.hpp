#pragma once

#include "../type-lookup.hpp"
#include "./type-binder.hpp"
#include "../class-typing.hpp"
#include "../concepts.hpp"
#include <memory>

namespace APICore
{

    template <TemplatedDataConcept TemplatedParameterType, class InheritedFrom>
    struct LuaBinderGenerator<TemplatedParameterType, InheritedFrom>
    {
        template <class ChildType = TemplatedParameterType>
        static void generateType(sol::state &state, sol::usertype<ChildType> *newType)
        {
            newType->set(sol::meta_function::construct, sol::factories(
                [](sol::object self, std::string param)
                {
                    return TemplatedParameterType(std::shared_ptr<std::string>(new std::string(param)));
                }));
        }
    };
}