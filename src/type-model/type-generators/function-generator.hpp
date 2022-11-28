#ifndef FUNCTION_TYPE_GENERATOR
#define FUNCTION_TYPE_GENERATOR

#include "../type-lookup.hpp"
#include "./type-generator.hpp"

namespace APICore
{
    template <typename ReturnType, typename... Parameters>
    struct TypeGenerator<ReturnType (Parameters...)>
    {
        static std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> generateTyping()
        {
            auto functionTyping = std::shared_ptr<
                TypeWrapper<
                    DataPrimitive::function
                >
            >(
                new TypeWrapper<DataPrimitive::function>(
                    "",
                    "",
                    std::vector<std::shared_ptr<APICore::TypeWrapper<APICore::unknown>>>(),
                    makeBasicType(DataPrimitive::unknown)
                )
            );

            auto returnType = TypeGenerator<ReturnType>::generateTyping();

            functionTyping->setReturnType(returnType);

            return functionTyping;
        }
    };

}

#endif