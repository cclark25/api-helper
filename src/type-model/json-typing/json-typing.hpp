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

    template <class T>
    struct JsonTypingGenerator
    {
        static std::shared_ptr<json> generateType()
        {
            std::shared_ptr<json> type = std::shared_ptr<json>(new json());
            
            return type;
        }
    };

    template <class T>
    struct JsonTyper
    {
        static std::shared_ptr<json> declaration;
        static std::shared_ptr<json> declareType();
    };

    template <class T>
    std::shared_ptr<json> JsonTyper<T>::declareType()
    {
        if (JsonTyper<T>::declaration == nullptr)
        {
            declaration = JsonTypingGenerator<T>::generateType();
            size_t id = setTypeId(declaration);
            typeMap.insert_or_assign(id, declaration);
        }
        return JsonTyper<T>::declaration;
    };

    template <class T>
    std::shared_ptr<json> JsonTyper<T>::declaration = nullptr;

    json generateTypeMap(){
        json map;
        for(auto mapping : typeMap){
            map[std::to_string(mapping.first)] = *mapping.second;
        }
        return map;
    }

    
}

#endif