#pragma once

#include <json.hpp>
#include <functional>
#include "../type-lookup.hpp"
#include "../member-function-pointer.hpp"

using json = nlohmann::json;

namespace APICore
{

    template <typename StaticFunctionDef, class ReturnType, class... Parameters>
    struct JsonTypingGenerator<ReturnType(Parameters...), StaticFunctionDef>
    {
        static void generateType(std::shared_ptr<json> type)
        {
            JsonTypingGenerator<ReturnType(Parameters...)>::generateType(type);
            std::string name = StaticFunctionDef::key;

            (*type)["name"] = name;
            (*type)["description"] = StaticFunctionDef::description;

            json parameters;

            size_t index = 0;

            ((
                 ([&index, &parameters]()
                  {
                        
                        if(StaticFunctionDef::isCustomMember && index == 0){
                            return;
                        }

                        json param;
                        auto p = StaticFunctionDef::parameterPack::parameters[index];
                        param["name"] = p.first;
                        param["description"] = p.second;
                        param["typeId"] = (*JsonTyper<Parameters>::declareType())["typeId"];

                        parameters[index++] = param; })()),
             ...);

            json functionDescription;
            functionDescription["parameters"] = parameters;
            functionDescription["returnTypeId"] = (*JsonTyper<ReturnType>::declareType())["typeId"];

            (*type)["functionDescription"] = functionDescription;
        }
    };

    template <typename StaticFunctionDef, class ReturnType, class... Parameters>
    struct JsonTyper<ReturnType(Parameters...), StaticFunctionDef>
    {
        static std::shared_ptr<json> declaration;

        static std::shared_ptr<json> declareType()
        {
            if (JsonTyper::declaration == nullptr)
            {
                JsonTyper::declaration = std::shared_ptr<json>(new json());
                size_t id = setTypeId(JsonTyper::declaration);
                JsonTypingGenerator<ReturnType(Parameters...), StaticFunctionDef>::generateType(JsonTyper::declaration);
                typeMap.insert_or_assign(id, JsonTyper::declaration);
            }
            return JsonTyper::declaration;
        };
    };

    template <typename StaticFunctionDef, class ReturnType, class... Parameters>
    std::shared_ptr<json> JsonTyper<ReturnType(Parameters...), StaticFunctionDef>::declaration = nullptr;



    template <typename StaticFunctionDef, class ReturnType, class... Parameters>
    struct JsonTypingGenerator<std::function<ReturnType(Parameters...)>, StaticFunctionDef>
    {
        static void generateType(std::shared_ptr<json> type)
        {
            JsonTypingGenerator<std::function<ReturnType(Parameters...)>>::generateType(type);
            std::string name = StaticFunctionDef::key;

            (*type)["name"] = name;
            (*type)["description"] = StaticFunctionDef::description;
            (*type)["isFunctionPointer"] = true;

            json parameters;

            size_t index = 0;

            ((
                 ([&index, &parameters]()
                  {
                        
                        if(StaticFunctionDef::isCustomMember && index == 0){
                            return;
                        }

                        json param;
                        auto p = StaticFunctionDef::parameterPack::parameters[index];
                        param["name"] = p.first;
                        param["description"] = p.second;
                        param["typeId"] = (*JsonTyper<Parameters>::declareType())["typeId"];

                        parameters[index++] = param; })()),
             ...);

            json functionDescription;
            functionDescription["parameters"] = parameters;
            functionDescription["returnTypeId"] = (*JsonTyper<ReturnType>::declareType())["typeId"];

            (*type)["functionDescription"] = functionDescription;
        }
    };

    template <typename StaticFunctionDef, class ReturnType, class... Parameters>
    struct JsonTyper<std::function<ReturnType(Parameters...)>, StaticFunctionDef>
    {
        static std::shared_ptr<json> declaration;

        static std::shared_ptr<json> declareType()
        {
            if (JsonTyper::declaration == nullptr)
            {
                JsonTyper::declaration = std::shared_ptr<json>(new json());
                size_t id = setTypeId(JsonTyper::declaration);
                JsonTypingGenerator<std::function<ReturnType(Parameters...)>, StaticFunctionDef>::generateType(JsonTyper::declaration);
                typeMap.insert_or_assign(id, JsonTyper::declaration);
            }
            return JsonTyper::declaration;
        };
    };

    template <typename StaticFunctionDef, class ReturnType, class... Parameters>
    std::shared_ptr<json> JsonTyper<std::function<ReturnType(Parameters...)>, StaticFunctionDef>::declaration = nullptr;

}
