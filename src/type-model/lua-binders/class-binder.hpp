#ifndef CLASS_LUA_BINDER
#define CLASS_LUA_BINDER

#include "../type-lookup.hpp"
#include "./type-binder.hpp"
#include "../class-typing.hpp"
#include <sol.hpp>

namespace APICore
{
    // template <typename T>
    // struct FieldTypings
    // {
    //     static std::map<std::string, std::shared_ptr<APICore::TypeWrapper<APICore::unknown>>> generateMemberTypings()
    //     {
    //         return std::map<std::string, std::shared_ptr<APICore::TypeWrapper<APICore::unknown>>>();
    //     }
    //     static std::map<std::string, std::shared_ptr<APICore::TypeWrapper<APICore::unknown>>> generateStaticFieldTypings()
    //     {
    //         return std::map<std::string, std::shared_ptr<APICore::TypeWrapper<APICore::unknown>>>();
    //     }
    // };

    // template <StringLiteral Name, StringLiteral Description, class ClassType, ClassField<ClassType>... Fields>
    // struct FieldTypings<ClassTyping<Name, Description, ClassType, Fields...>>
    // {
    //     static std::map<std::string, std::shared_ptr<APICore::TypeWrapper<APICore::unknown>>> generateMemberTypings()
    //     {
    //         std::map<std::string, std::shared_ptr<APICore::TypeWrapper<APICore::unknown>>> results;

    //         ((
    //              MemberPtrSpec<Fields> ? [&results]()
    //                  {results.insert_or_assign(Fields::key, LuaBinder<typename Fields::type, "MEMBER", Fields>::generateTyping()); return true; }()
    //                                    : false),
    //          ...);

    //         ((
    //              MemberPtrSpec<Fields> ? [&results]()
    //                  { results.at(Fields::key)->name = (Fields::key); return true; }()
    //                                    : false),
    //          ...);

    //         ((
    //              MemberPtrSpec<Fields> ? [&results]()
    //                  {results.at(Fields::key)->description = (Fields::description); return true; }()
    //                                    : false),
    //          ...);

    //         return results;
    //     }
    //     static std::map<std::string, std::shared_ptr<APICore::TypeWrapper<APICore::unknown>>> generateStaticFieldTypings()
    //     {
    //         std::map<std::string, std::shared_ptr<APICore::TypeWrapper<APICore::unknown>>> results;

    //         ((
    //              StaticPtrSpec<Fields> ? [&results]()
    //                  {results.insert_or_assign(Fields::key, LuaBinder<typename Fields::type, "STATIC", Fields>::generateTyping()); return true; }()
    //                                    : false),
    //          ...);

    //         ((
    //              StaticPtrSpec<Fields> ? [&results]()
    //                  { results.at(Fields::key)->name = (Fields::key); return true; }()
    //                                    : false),
    //          ...);

    //         ((
    //              StaticPtrSpec<Fields> ? [&results]()
    //                  {results.at(Fields::key)->description = (Fields::description); return true; }()
    //                                    : false),
    //          ...);

    //         return results;
    //     }
    // };

    template <class ClassType, class... ExtraData>
    struct LuaBinder<ClassType, "ANY", ExtraData...>
    {
        using type_definition = TypeLookup<ClassType>;

        static void bind(sol::state &state)
        {
            sol::usertype<ClassType> newClassType = state.new_usertype<ClassType>(
                type_definition::registeredType::name,
                sol::constructors<ClassType()>());
            
            newClassType["testField"] = [](){ return 12; };
            
            // TODO: add support for defining constructors

            // auto instanceType = std::shared_ptr<ObjectTypeWrapper>(
            //     new ObjectTypeWrapper(
            //         std::string(type_definition::registeredType::name) + "InstanceType",
            //         "The type of objects that are instances of the " + std::string(type_definition::registeredType::name) + " class.",
            //         FieldTypings<typename type_definition::registeredType>::generateMemberTypings()));
            // auto staticType = std::shared_ptr<ObjectTypeWrapper>(
            //     new ObjectTypeWrapper(
            //         std::string(type_definition::registeredType::name) + "StaticType",
            //         "The type of the static fields of the " + std::string(type_definition::registeredType::name) + " class.",
            //         FieldTypings<typename type_definition::registeredType>::generateStaticFieldTypings()));
            // auto resultType = std::shared_ptr<TypeWrapper<DataPrimitive::classType>>(
            //     new TypeWrapper<DataPrimitive::classType>(
            //         type_definition::registeredType::name,
            //         type_definition::registeredType::description,
            //         staticType,
            //         std::vector<std::shared_ptr<APICore::TypeWrapper<APICore::unknown>>>()));
            // resultType->setInstanceType(instanceType);

            // return resultType;
        }
    };
}

#endif