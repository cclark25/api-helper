#ifndef CLASS_LUA_JSON_TYPING
#define CLASS_LUA_JSON_TYPING

#include <json.hpp>
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

    template <StringLiteral Name, StringLiteral Description, class ClassType, class InheritedFrom, ClassField<ClassType>... Fields>
    struct JSONFieldTypings<ClassTyping<Name, Description, ClassType, InheritedFrom, Fields...>>
    {
        static json generateMemberTypings()
        {
            json results;


            ((
                 MemberPtrSpec<Fields> ? [&results]()
                     {
                        std::string key = Fields::key;
                        std::shared_ptr<json> typeDef = JsonTyper<typename Fields::type, Fields>::declareType();
                        json fieldDef;
                        fieldDef["typeId"] = (*typeDef)["typeId"]; 
                        fieldDef["description"] = Fields::description;
                        
                        results[key] = fieldDef;
                        return true; }()
                                       : false),
             ...);

            return results;
        }
        static json generateStaticFieldTypings()
        {
            json results;

            ((
                 StaticPtrSpec<Fields> ? [&results]()
                     {
                        std::string key = Fields::key;
                        std::shared_ptr<json> typeDef = JsonTyper<typename Fields::type, Fields>::declareType();
                        json fieldDef;
                        fieldDef["typeId"] = (*typeDef)["typeId"]; 
                        fieldDef["description"] = Fields::description;
                        
                        results[Fields::key] = fieldDef;
                        return true; }()
                                       : false),
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

            if constexpr (!std::is_void_v<typename type_definition::registeredType::inheritedFrom>){
                using ParentType = type_definition::registeredType::inheritedFrom;
                auto parentType = JsonTyper<ParentType>::declareType();
                (*type)["inheritsFrom"] = (*parentType)["typeId"];
            }
        }
    };
}

#endif