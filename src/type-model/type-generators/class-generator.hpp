#ifndef CLASS_TYPE_GENERATOR
#define CLASS_TYPE_GENERATOR

#include "../type-lookup.hpp"
#include "./type-generator.hpp"
#include "../../type-wrapper/type-wrapper.hpp"
#include "../class-typing.hpp"

namespace APICore
{
    template <typename T>
    struct FieldTypings
    {
        static std::map<std::string, std::shared_ptr<APICore::TypeWrapper<APICore::unknown>>> generateMemberTypings()
        {
            return std::map<std::string, std::shared_ptr<APICore::TypeWrapper<APICore::unknown>>>();
        }
    };

    template <StringLiteral Name, StringLiteral Description, class ClassType, ClassField<ClassType>... Fields>
    struct FieldTypings<ClassTyping<Name, Description, ClassType, Fields...>>
    {
        static std::map<std::string, std::shared_ptr<APICore::TypeWrapper<APICore::unknown>>> generateMemberTypings()
        {
            std::map<std::string, std::shared_ptr<APICore::TypeWrapper<APICore::unknown>>> results;

            ((
                results.insert_or_assign(Fields::key, TypeGenerator<typename Fields::type>::generateTyping() )
             ),
             ...);

            return results;
        }
    };

    template <class ClassType>
    struct TypeGenerator
    {
        using type_definition = TypeLookup<ClassType>;

        static std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> generateTyping()
        {
            auto instanceType = std::shared_ptr<ObjectTypeWrapper>(
                new ObjectTypeWrapper(
                    "",
                    "",
                    FieldTypings<typename type_definition::registeredType>::generateMemberTypings()));
            auto staticType = std::shared_ptr<ObjectTypeWrapper>(
                new ObjectTypeWrapper(
                    "",
                    "",
                    std::map<std::string, std::shared_ptr<APICore::TypeWrapper<APICore::unknown>>>()));
            auto resultType = std::shared_ptr<TypeWrapper<DataPrimitive::classType>>(
                new TypeWrapper<DataPrimitive::classType>(
                    type_definition::registeredType::name,
                    type_definition::registeredType::description,
                    staticType,
                    std::vector<std::shared_ptr<APICore::TypeWrapper<APICore::unknown>>>()
                    ));
            resultType->setInstanceType(instanceType);

            return resultType;
        }
    };
}

#endif