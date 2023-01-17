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
        size_t id = (size_t)ptr.get();
        (*ptr)["typeId"] = std::to_string(id);
        return id;
    }

    template <class T, class... ExtraData>
    struct JsonTypingGenerator : public JsonTypingGenerator<T>
    {
    };

    template <class T>
    struct JsonTypingGenerator<T>
    {
        static void generateType(std::shared_ptr<json> type)
        {
            (*type)["name"] = "unknown";
            (*type)["isBasicType"] = true;
        }
    };

    template <>
    struct JsonTypingGenerator<double>
    {
        static void generateType(std::shared_ptr<json> type)
        {
            (*type)["name"] = "double";
            (*type)["isBasicType"] = true;
        }
    };

    template <>
    struct JsonTypingGenerator<int>
    {
        static void generateType(std::shared_ptr<json> type)
        {
            (*type)["name"] = "int";
            (*type)["isBasicType"] = true;
        }
    };
    template <>
    struct JsonTypingGenerator<std::string>
    {
        static void generateType(std::shared_ptr<json> type)
        {
            (*type)["name"] = "string";
            (*type)["isBasicType"] = true;
        }
    };
    template <>
    struct JsonTypingGenerator<bool>
    {
        static void generateType(std::shared_ptr<json> type)
        {
            (*type)["name"] = "bool";
            (*type)["isBasicType"] = true;
        }
    };

    template <>
    struct JsonTypingGenerator<char>
    {
        static void generateType(std::shared_ptr<json> type)
        {
            (*type)["name"] = "char";
            (*type)["isBasicType"] = true;
        }
    };

    template <>
    struct JsonTypingGenerator<void>
    {
        static void generateType(std::shared_ptr<json> type)
        {
            (*type)["name"] = "void";
            (*type)["isBasicType"] = true;
        }
    };

    template <class T, class... ExtraData>
    struct JsonTyper : public JsonTyper<T>
    {
        static std::shared_ptr<json> declareType()
        {
            if (JsonTyper::declaration == nullptr)
            {
                JsonTyper::declaration = std::shared_ptr<json>(new json());
                size_t id = setTypeId(JsonTyper::declaration);
                JsonTypingGenerator<T, ExtraData...>::generateType(JsonTyper::declaration);
                typeMap.insert_or_assign(id, JsonTyper::declaration);
            }
            return JsonTyper::declaration;
        };
    };

    template <class T>
    struct JsonTyper<T>
    {
        static std::shared_ptr<json> declaration;

        static std::shared_ptr<json> declareType()
        {
            if (JsonTyper::declaration == nullptr)
            {
                JsonTyper::declaration = std::shared_ptr<json>(new json());
                size_t id = setTypeId(JsonTyper::declaration);
                JsonTypingGenerator<T>::generateType(JsonTyper::declaration);
                typeMap.insert_or_assign(id, JsonTyper::declaration);
            }
            return JsonTyper::declaration;
        };
    };

    template <class T>
    std::shared_ptr<json> JsonTyper<T>::declaration = nullptr;

    json generateTypeMap()
    {
        json map;
        for (auto mapping : typeMap)
        {
            map[std::to_string(mapping.first)] = *mapping.second;
        }
        return map;
    }

}

#include "./class-json-typing.hpp"
#include "./function-json-typing.hpp"
#include "./reference-json-typing.hpp"

#endif