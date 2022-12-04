#ifndef FUNCTION_TYPE_GENERATOR
#define FUNCTION_TYPE_GENERATOR

#include "../type-lookup.hpp"
#include "../member-function-pointer.hpp"
#include "./type-generator.hpp"
#include <iostream>

namespace APICore
{
    template <class MemberFunctionDef, typename ReturnType, typename... Parameters>
    struct TypeGenerator<ReturnType(Parameters...), "MEMBER", MemberFunctionDef>
    {
        static std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> generateTyping()
        {
            auto functionTyping = std::shared_ptr<
                TypeWrapper<
                    DataPrimitive::function>>(
                new TypeWrapper<DataPrimitive::function>(
                    "",
                    "",
                    std::vector<std::shared_ptr<APICore::TypeWrapper<APICore::unknown>>>(),
                    makeBasicType(DataPrimitive::unknown)));

            auto returnType = TypeGenerator<ReturnType>::generateTyping();

            functionTyping->setReturnType(returnType);

            ((functionTyping->parameters.push_back(TypeGenerator<Parameters>::generateTyping())), ...);

            // auto parameters = MemberFunctionParameters<Pointer>;

            // for(auto p : MemberFunctionDef::parameterPack::parameters){
            //     std::cout << "Function parameter: " << p.first << " - " << p.second << std::endl;
            // }

            size_t index = 0;
            for(auto p : MemberFunctionDef::parameterPack::parameters){
                functionTyping->parameters[index]->name = p.first;
                functionTyping->parameters[index]->description = p.second;
                index++;
            }

            return functionTyping;
        }
    };

    template <typename T, typename... ExtraData>
    struct TypeGenerator<T, "MEMBER", ExtraData...> : public TypeGenerator<T, "ANY", ExtraData...>
    {
    };
}

#endif