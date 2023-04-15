#pragma once


#include <json.hpp>
#include "../concepts.hpp"
#include "../templated-types.hpp"

using json = nlohmann::json;

namespace APICore
{
    template <StringLiteral PName>
    struct JsonTypingGenerator<TemplateParam<PName>>
    {
        static void generateType(std::shared_ptr<json> type)
        {
            (*type)["name"] = "templateParameterValue";
            (*type)["description"] = "An instance of a type defined by a template parameter.";
            (*type)["isTemplateParameterType"] = true;

            (*type)["resolvingTemplateParameterName"] = std::string(PName.value);
        }
    };
}