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
        static std::map<std::string, std::shared_ptr<APICore::TypeWrapper<APICore::unknown>>> generateStaticFieldTypings()
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
                 MemberPtrSpec<Fields> ? [&results]()
                     {results.insert_or_assign(Fields::key, TypeGenerator<typename Fields::type, "MEMBER", Fields>::generateTyping()); return true; }()
                                       : false),
             ...);

            ((
                 MemberPtrSpec<Fields> ? [&results]()
                     { results.at(Fields::key)->name = (Fields::key); return true; }()
                                       : false),
             ...);

            ((
                 MemberPtrSpec<Fields> ? [&results]()
                     {results.at(Fields::key)->description = (Fields::description); return true; }()
                                       : false),
             ...);

            return results;
        }
        static std::map<std::string, std::shared_ptr<APICore::TypeWrapper<APICore::unknown>>> generateStaticFieldTypings()
        {
            std::map<std::string, std::shared_ptr<APICore::TypeWrapper<APICore::unknown>>> results;

            ((
                 StaticPtrSpec<Fields> ? [&results]()
                     {results.insert_or_assign(Fields::key, TypeGenerator<typename Fields::type>::generateTyping()); return true; }()
                                       : false),
             ...);

            ((
                 StaticPtrSpec<Fields> ? [&results]()
                     { results.at(Fields::key)->name = (Fields::key); return true; }()
                                       : false),
             ...);

            ((
                 StaticPtrSpec<Fields> ? [&results]()
                     {results.at(Fields::key)->description = (Fields::description); return true; }()
                                       : false),
             ...);

            return results;
        }
    };

    template <class ClassType, class... ExtraData>
    struct TypeGenerator<ClassType, "ANY", ExtraData...>
    {
        using type_definition = TypeLookup<ClassType>;

        static std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> generateTyping()
        {
            auto instanceType = std::shared_ptr<ObjectTypeWrapper>(
                new ObjectTypeWrapper(
                    std::string(type_definition::registeredType::name) + "InstanceType",
                    "The type of objects that are instances of the " + std::string(type_definition::registeredType::name) + " class.",
                    FieldTypings<typename type_definition::registeredType>::generateMemberTypings()));
            auto staticType = std::shared_ptr<ObjectTypeWrapper>(
                new ObjectTypeWrapper(
                    std::string(type_definition::registeredType::name) + "StaticType",
                    "The type of the static fields of the " + std::string(type_definition::registeredType::name) + " class.",
                    FieldTypings<typename type_definition::registeredType>::generateStaticFieldTypings()));
            auto resultType = std::shared_ptr<TypeWrapper<DataPrimitive::classType>>(
                new TypeWrapper<DataPrimitive::classType>(
                    type_definition::registeredType::name,
                    type_definition::registeredType::description,
                    staticType,
                    std::vector<std::shared_ptr<APICore::TypeWrapper<APICore::unknown>>>()));
            resultType->setInstanceType(instanceType);

            return resultType;
        }
    };
}

#endif