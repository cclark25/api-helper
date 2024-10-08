#pragma once

#include <json.hpp>
#include "../class-typing.hpp"
#include "../concepts.hpp"

using json = nlohmann::json;

namespace APICore
{
    template <ReferenceTypeConcept RefType>
    struct JsonTypingGenerator<RefType>
    {
        using type_definition = TypeLookup<RefType>;
        static void generateType(std::shared_ptr<json> type)
        {
            std::shared_ptr<json> refType = JsonTyper<typename dereference<RefType>::type>::declareType();

            (*type)["name"] = "reference";
            (*type)["description"] = "";
            (*type)["referenceTypeId"] = (*refType)["typeId"];
        }
    };
}
