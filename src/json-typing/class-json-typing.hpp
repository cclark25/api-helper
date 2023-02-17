#pragma once

#include <json.hpp>
#include <vector>
#include <iostream>
#include "../class-typing.hpp"
#include "../concepts.hpp"

using json = nlohmann::json;

namespace APICore
{
    template <typename T>
    struct JSONFieldTypings
    {
        static json generateMemberTypings()
        {
            return json();
        }
        static json generateStaticFieldTypings()
        {
            return json();
        }
    };

    template <MemberPtrSpec Field>
    json bindMember(json &dest)
    {
        std::string key = Field::key;
        std::shared_ptr<json> typeDef = JsonTyper<typename Field::type, Field>::declareType();
        json fieldDef;
        fieldDef["typeId"] = (*typeDef)["typeId"];
        fieldDef["description"] = Field::description;
        dest[key] = fieldDef;
        return fieldDef;
    }
    template <StaticPtrSpec Field>
    json bindStatic(json &dest)
    {
        std::string key = Field::key;
        std::shared_ptr<json> typeDef = JsonTyper<typename Field::type, Field>::declareType();
        json fieldDef;
        fieldDef["typeId"] = (*typeDef)["typeId"];
        fieldDef["description"] = Field::description;
        dest[key] = fieldDef;
        return fieldDef;
    }
    template <MemberOverloadSpec Field>
    struct MemberOverloader
    {
        static json bindMemberOverloads(json &dest);
    };

    template <MemberFunctionPtrSpec... Members>
    struct MemberOverloader<MemberOverload<Members...>>
    {
        static json bindMemberOverloads(json &dest)
        {
            std::string key = MemberOverload<Members...>::getKey();
            json fieldDef;
            json overloads;

            fieldDef["isMemberOverload"] = true;

            fieldDef["overloads"] = std::vector<json>({bindMember<Members>(overloads)...});
            dest[key] = fieldDef;
            return fieldDef;
        }
    };

    template <StaticOverloadSpec Field>
    struct StaticOverloader
    {
        static json bindStaticOverloads(json &dest);
    };

    template <StaticFunctionPtrSpec... Members>
    struct StaticOverloader<StaticOverload<Members...>>
    {
        static json bindStaticOverloads(json &dest)
        {
            std::string key = StaticOverload<Members...>::getKey();
            json fieldDef;
            json overloads;

            fieldDef["isStaticOverload"] = true;

            fieldDef["overloads"] = std::vector<json>({bindStatic<Members>(overloads)...});
            dest[key] = fieldDef;
            return fieldDef;
        }
    };

    template <StringLiteral Name, StringLiteral Description, class ClassType, class InheritedFrom, ClassField<ClassType>... Fields>
    struct JSONFieldTypings<ClassTyping<Name, Description, ClassType, InheritedFrom, Fields...>>
    {
        static json generateMemberTypings()
        {
            json results;

            ((
                 [&results]()
                 {
                     if constexpr (MemberPtrSpec<Fields>)
                     {
                         bindMember<Fields>(results);
                         return true;
                     }
                     else if constexpr (MemberOverloadSpec<Fields>)
                     {
                         MemberOverloader<Fields>::bindMemberOverloads(results);
                         return true;
                     }
                     return false;
                 }()),
             ...);

            return results;
        }
        static json generateStaticFieldTypings()
        {
            json results;

            ((
                 [&results]()
                 {
                     if constexpr (StaticPtrSpec<Fields>)
                     {
                         bindStatic<Fields>(results);
                         return true;
                     }
                     else if constexpr (StaticOverloadSpec<Fields>)
                     {
                         StaticOverloader<Fields>::bindStaticOverloads(results);
                         return true;
                     }
                     return false;
                 }()),
             ...);

            return results;
        }
    };

    template <ClassTypeConcept ClassType>
    struct JsonTypingGenerator<ClassType>
    {
        using type_definition = TypeLookup<ClassType>;
        static void generateType(std::shared_ptr<json> type)
        {

            (*type)["name"] = type_definition::registeredType::name;
            (*type)["description"] = type_definition::registeredType::description;

            json staticFields = JSONFieldTypings<typename type_definition::registeredType>::generateStaticFieldTypings();
            json memberFields = JSONFieldTypings<typename type_definition::registeredType>::generateMemberTypings();

            (*type)["staticFields"] = staticFields;
            (*type)["memberFields"] = memberFields;

            if constexpr (!std::is_void_v<typename type_definition::registeredType::inheritedFrom>)
            {
                using ParentType = type_definition::registeredType::inheritedFrom;
                auto parentType = JsonTyper<ParentType>::declareType();
                (*type)["inheritsFrom"] = (*parentType)["typeId"];
            }
        }
    };
}
