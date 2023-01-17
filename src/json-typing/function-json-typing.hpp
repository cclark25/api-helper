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
        static void generateType(std::shared_ptr<json> type)
        {
            JsonTypingGenerator<ReturnType(Parameters...)>::generateType(type);

            (*type)["name"] = StaticFunctionDef::key;
            (*type)["description"] = StaticFunctionDef::description;

            json parameters;

            size_t index = 0;

            ((
                 ([&index, &parameters]()
                  {
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
}

#endif