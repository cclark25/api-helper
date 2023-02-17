#pragma once

#include <memory>
#include <string>
#include <map>
#include <json.hpp>

using json = nlohmann::json;

#define DeclareBasicType(typename)                           \
    template <>                                              \
    struct JsonTypingGenerator<typename>                     \
    {                                                        \
        static void generateType(std::shared_ptr<json> type) \
        {                                                    \
            (*type)["name"] = #typename;                     \
            (*type)["isBasicType"] = true;                   \
        }                                                    \
    }

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

    DeclareBasicType(double);
    DeclareBasicType(int);
    DeclareBasicType(size_t);
    using string = std::string;
    DeclareBasicType(string);
    DeclareBasicType(bool);
    DeclareBasicType(char);
    DeclareBasicType(void);

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
