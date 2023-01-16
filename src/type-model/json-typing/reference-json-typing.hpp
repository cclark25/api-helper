#ifndef REFERENCE_LUA_JSON_TYPING
#define REFERENCE_LUA_JSON_TYPING

#include <json.hpp>
#include "../class-typing.hpp"
#include "../concepts.hpp"

using json = nlohmann::json;

namespace APICore
{
    template <ReferenceTypeConcept RefType, class... ExtraData>
    struct JsonTypingGenerator<RefType, ExtraData...>
    {
        using type_definition = TypeLookup<RefType>;
        static std::shared_ptr<json> generateType()
        {
            std::shared_ptr<json> type = std::shared_ptr<json>(new json());
            std::shared_ptr<json> refType = JsonTyper<typename dereference<RefType>::type>::declareType();

            (*type)["name"] = "";
            (*type)["description"] = "";
            (*type)["referenceTypeId"] = (*refType)["typeId"];


            return type;
        }
    };
}

#endif