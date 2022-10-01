#ifndef __TEST_FUNCTION_DEFINITION
#define __TEST_FUNCTION_DEFINITION
#include <memory>
#include "../../../src/lua-binding/binding-types/all.hpp"
#include "../../../src/data-wrappers/data-wrapper.hpp"

using namespace APICore;

data_primitive_to_type<DataPrimitive::function> functionInternalValue = {
    [](FunctionInternalType::FunctionParams params)
    {
        return std::shared_ptr<StringContainerWrapper>(new StringContainerWrapper("functionValue return value."));
    }};
auto parameterVector = std::vector<std::shared_ptr<APICore::TypeWrapper<APICore::unknown>>>(
    {std::shared_ptr<TypeWrapper<APICore::unknown>>(new TypeWrapper<DataPrimitive::string>("param1", "Parameter 1"))});

auto returnObjectType = std::shared_ptr<APICore::TypeWrapper<APICore::object>>(
    new TypeWrapper<APICore::object>(
        "functionInternalValueReturnValueType",
        "Return type description.",
        std::map<std::string, std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>>(
            {{"f1",
              std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>(new TypeWrapper<DataPrimitive::string>("param1", "Parameter 1 again"))}})));

auto functionExampleDefinition = std::shared_ptr<FunctionWrapper>(
    new FunctionContainerWrapper(
        functionInternalValue,
        std::shared_ptr<TypeWrapper<DataPrimitive::function>>(
            new TypeWrapper<DataPrimitive::function>(
                "testFunctionName",
                "A Test Function",
                parameterVector,
                returnObjectType))));
#endif