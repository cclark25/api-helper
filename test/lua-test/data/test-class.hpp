#ifndef __TEST_CLASS_DEFINITION
#define __TEST_CLASS_DEFINITION
#include <memory>
#include "../../../src/lua-binding/binding-types/all.hpp"
#include "../../../src/data-wrappers/data-wrapper.hpp"

using namespace APICore;

data_primitive_to_type<DataPrimitive::function> classConstructorFunction = {
    [](FunctionInternalType::FunctionParams params)
    {
        auto obj = std::shared_ptr<ObjectWrapper>(new ObjectContainerWrapper());
        obj->setField(
            "f1",
            std::shared_ptr<StringWrapper>(new StringContainerWrapper("Member string.")));
        return obj;
    }};

auto classTyping = std::shared_ptr<ClassTypeWrapper>(new ClassTypeWrapper(
    "TestClass", "A Test Class", std::shared_ptr<ObjectTypeWrapper>(new ObjectTypeWrapper("testClassStaticFields", "Static fields for TestClass", std::map<std::string, std::shared_ptr<APICore::TypeWrapper<APICore::unknown>>>(
        {
            {"s3", makeBasicType<DataPrimitive::string>()}
        }
    ))),
    std::vector<std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>>(),
    nullptr));

auto classInstanceType = std::shared_ptr<TypeWrapper<APICore::classInstance>>(
    new ClassInstanceTypeWrapper(
        classTyping,
        std::map<std::string, std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>>(
            {
                {"f1", makeBasicType<DataPrimitive::string>()}
            }
        )));

auto classConstructor = std::shared_ptr<FunctionWrapper>(
    new FunctionContainerWrapper(
        classConstructorFunction, std::shared_ptr<TypeWrapper<DataPrimitive::function>>(
                                      new TypeWrapper<DataPrimitive::function>(
                                          "testFunctionName",
                                          "A Test Function",
                                          std::vector<std::shared_ptr<APICore::TypeWrapper<APICore::unknown>>>(),
                                          classInstanceType))));

auto classStaticFields = std::map<std::string, std::shared_ptr<DataWrapper>>();
auto x = classStaticFields.insert_or_assign(
    "s1",
    std::shared_ptr<StringWrapper>(new StringContainerWrapper("Static field string.")));

auto classDefinition = std::shared_ptr<ClassWrapper>(new ClassContainerWrapper({"TestClass", classConstructor, classStaticFields}, classTyping));

#endif