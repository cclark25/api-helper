#ifndef BASE_LUA_JSON_TYPING
#define BASE_LUA_JSON_TYPING

#include <memory>
#include <string>
#include <map>
#include <json.hpp>

using json = nlohmann::json;

namespace APICore
{
    std::map<size_t, std::shared_ptr<json>> typeMap;

    size_t setTypeId(std::shared_ptr<json> ptr)
    {
        size_t id = (size_t) ptr.get();
        (*ptr)["typeId"] = std::to_string(id);
        return id;
    }

    template <class T, class... ExtraData>
    struct JsonTypingGenerator
    {
        static std::shared_ptr<json> generateType()
        {
            std::shared_ptr<json> type = std::shared_ptr<json>(new json());

            (*type)["name"] = "unknown";
            (*type)["isBasicType"] = true;
            
            return type;
        }
    };

    template <class... ExtraData>
    struct JsonTypingGenerator<double, ExtraData...>
    {
        static std::shared_ptr<json> generateType()
        {
            std::shared_ptr<json> type = std::shared_ptr<json>(new json());

            (*type)["name"] = "double";
            (*type)["isBasicType"] = true;
            
            return type;
        }
    };

    template <class... ExtraData>
    struct JsonTypingGenerator<int, ExtraData...>
    {
        static std::shared_ptr<json> generateType()
        {
            std::shared_ptr<json> type = std::shared_ptr<json>(new json());

            (*type)["name"] = "int";
            (*type)["isBasicType"] = true;
            
            return type;
        }
    };
    template <class... ExtraData>
    struct JsonTypingGenerator<std::string, ExtraData...>
    {
        static std::shared_ptr<json> generateType()
        {
            std::shared_ptr<json> type = std::shared_ptr<json>(new json());

            (*type)["name"] = "string";
            (*type)["isBasicType"] = true;
            
            return type;
        }
    };
    template <class... ExtraData>
    struct JsonTypingGenerator<bool, ExtraData...>
    {
        static std::shared_ptr<json> generateType()
        {
            std::shared_ptr<json> type = std::shared_ptr<json>(new json());

            (*type)["name"] = "bool";
            (*type)["isBasicType"] = true;
            
            return type;
        }
    };

    template <class... ExtraData>
    struct JsonTypingGenerator<char, ExtraData...>
    {
        static std::shared_ptr<json> generateType()
        {
            std::shared_ptr<json> type = std::shared_ptr<json>(new json());

            (*type)["name"] = "char";
            (*type)["isBasicType"] = true;
            
            return type;
        }
    };

    template <class... ExtraData>
    struct JsonTypingGenerator<void, ExtraData...>
    {
        static std::shared_ptr<json> generateType()
        {
            std::shared_ptr<json> type = std::shared_ptr<json>(new json());

            (*type)["name"] = "void";
            (*type)["isBasicType"] = true;
            
            return type;
        }
    };

        

    template <class T, class... ExtraData>
    struct JsonTyper
    {
        static std::shared_ptr<json> declaration;
        static std::shared_ptr<json> declareType();
    };

    template <class T, class... ExtraData>
    std::shared_ptr<json> JsonTyper<T, ExtraData...>::declareType()
    {
        if (declaration == nullptr)
        {
            declaration = JsonTypingGenerator<T, ExtraData...>::generateType();
            size_t id = setTypeId(declaration);
            typeMap.insert_or_assign(id, declaration);
        }
        return declaration;
    };

    template <class T, class... ExtraData>
    std::shared_ptr<json> JsonTyper<T, ExtraData...>::declaration = nullptr;

    json generateTypeMap(){
        json map;
        for(auto mapping : typeMap){
            map[std::to_string(mapping.first)] = *mapping.second;
        }
        return map;
    }

    
}

#include "./class-json-typing.hpp"
#include "./function-json-typing.hpp"
#include "./reference-json-typing.hpp"

#endif