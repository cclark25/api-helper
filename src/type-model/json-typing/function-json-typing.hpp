#ifndef FUNCTION_LUA_JSON_TYPING
#define FUNCTION_LUA_JSON_TYPING

#include <json.hpp>
#include "../type-lookup.hpp"
#include "../member-function-pointer.hpp"

using json = nlohmann::json;

namespace APICore
{

    template <typename StaticFunctionDef, class ReturnType, class... Parameters>
    struct JsonTypingGenerator<ReturnType(Parameters...), StaticFunctionDef>
    {
        static std::shared_ptr<json> generateType()
        {
            std::shared_ptr<json> type = std::shared_ptr<json>(new json());

            (*type)["name"] = StaticFunctionDef::key;
            (*type)["description"] = StaticFunctionDef::description;

            json parameters;

            size_t index = 0;

            ((
                 ([&index, &parameters]()
                  {
                        json param;
                        param["name"] = StaticFunctionDef::parameterPack::parameters[index].first;
                        param["description"] = StaticFunctionDef::parameterPack::parameters[index].second;
                        param["typeId"] = (*JsonTyper<Parameters>::declareType())["typeId"];

                        parameters[index++] = param; })()),
             ...);

            // for(auto p : StaticFunctionDef::parameterPack::parameters){
            //     json param;
            //     param["name"] = p.first;
            //     param["description"] = p.second;
            //     param["typeId"] = JsonTyper<>;

            //     parameters[index] = param;
            //     index++;
            // }

            json functionDescription;
            functionDescription["parameters"] = parameters;

            (*type)["functionDescription"] = functionDescription;

            return type;
        }
    };
}

#endif