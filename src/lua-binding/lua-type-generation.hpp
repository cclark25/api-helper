#ifndef LUA_TYPE_GENERATION
#define LUA_TYPE_GENERATION

#include <map>
#include <memory>
#include <sol.hpp>
#include "../data-wrappers/data-wrapper.hpp"
#include "./binding-types/create-binding-object.h"
#include "./lua-loader.hpp"
#include <json.hpp>

namespace APILua
{
    using namespace APICore;
    using json = nlohmann::json;

    template <DataPrimitive D>
    std::map<std::shared_ptr<TypeWrapper<D>>, json> existingTypeMappings;

    template <DataPrimitive D>
    json makeTypingObjectFromTypeDefinition(sol::state &state, std::shared_ptr<TypeWrapper<D>> typing);
    template <>
    json makeTypingObjectFromTypeDefinition(sol::state &state, std::shared_ptr<TypeWrapper<DataPrimitive::function>> typing);
    template <>
    json makeTypingObjectFromTypeDefinition(sol::state &state, std::shared_ptr<TypeWrapper<DataPrimitive::object>> typing);
    template <>
    json makeTypingObjectFromTypeDefinition(sol::state &state, std::shared_ptr<TypeWrapper<DataPrimitive::array>> typing);
    template <>
    json makeTypingObjectFromTypeDefinition(sol::state &state, std::shared_ptr<TypeWrapper<DataPrimitive::classType>> typing);
    // template <>
    // json makeTypingObjectFromTypeDefinition(sol::state &state, std::shared_ptr<TypeWrapper<DataPrimitive::classInstance>> typing);

    template <DataPrimitive D>
    json makeTypingObjectFromDataWrapper(sol::state &state, std::shared_ptr<DataWrapperSub<D>> data);
    template <>
    json makeTypingObjectFromDataWrapper(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::object>> data);
    template <>
    json makeTypingObjectFromDataWrapper(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::classType>> data);

    template <DataPrimitive D>
    json makeTypingObject(sol::state &state, std::shared_ptr<DataWrapperSub<D>> data)
    {
        auto typingPtr = data->getType();
        if (typingPtr == nullptr)
        {
            return makeTypingObjectFromDataWrapper<D>(state, data);
        }
        else
        {
            auto castPtr = CastSharedPtr(TypeWrapper<D>, typingPtr);
            auto result = makeTypingObjectFromTypeDefinition<D>(state, castPtr);
            return result;
        }
    }

    template <DataPrimitive D>
    json makeTypingObjectFromTypeDefinition(sol::state &state, std::shared_ptr<TypeWrapper<D>> typing)
    {
        json typeDefinitionTable;
        DataPrimitive prim = typing->getPrimitiveType();
        typeDefinitionTable["dataPrimitive"] = dataPrimitiveNameMap.at(prim);
        typeDefinitionTable["name"] = typing->getTypeName();
        typeDefinitionTable["readonly"] = typing->isReadonly();
        typeDefinitionTable["description"] = typing->getDescription();

        return typeDefinitionTable;
    }

    template <>
    json makeTypingObjectFromTypeDefinition<DataPrimitive::unknown>(sol::state &state, std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> typing)
    {
        DataPrimitive prim = typing == nullptr ? DataPrimitive::unknown : typing->getPrimitiveType();

        if (prim == DataPrimitive::unknown)
        {
            auto typeDefinitionTable = json();
            typeDefinitionTable["dataPrimitive"] = dataPrimitiveNameMap.at(prim);
            typeDefinitionTable["name"] = typing->getTypeName();
            typeDefinitionTable["readonly"] = typing->isReadonly();
            typeDefinitionTable["description"] = typing->getDescription();

            return typeDefinitionTable;
        }

#define D(Primitive, Type)                                                                                                                \
    case DataPrimitive::Primitive:                                                                                                        \
    {                                                                                                                                     \
        std::shared_ptr<TypeWrapper<DataPrimitive::Primitive>> castTyping = CastSharedPtr(TypeWrapper<DataPrimitive::Primitive>, typing); \
        return makeTypingObjectFromTypeDefinition<DataPrimitive::Primitive>(state, castTyping);                                           \
    }                                                                                                                                     \
    break;

        switch (prim)
        {
            DATA_PRIMITIVE_VALUES
        }

        throw "Type generation failed for unknown type!";
#undef D
    }

    template <>
    json makeTypingObjectFromTypeDefinition(sol::state &state, std::shared_ptr<TypeWrapper<DataPrimitive::function>> typing)
    {
        auto asUnknown = std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>(new TypeWrapper<DataPrimitive::unknown>(typing));
        json typeDefinitionTable = makeTypingObjectFromTypeDefinition<DataPrimitive::unknown>(state, asUnknown);
        typeDefinitionTable["dataPrimitive"] = dataPrimitiveNameMap.at(DataPrimitive::function);
        typeDefinitionTable["name"] = typing->getTypeName();

        json functionDefinition = json();
        std::vector<json> parameters;
        json returnType = makeTypingObjectFromTypeDefinition<DataPrimitive::unknown>(state, typing->getReturnType());

        for (auto param : typing->getParams())
        {
            json paramElement = json();
            json paramTyping = makeTypingObjectFromTypeDefinition<DataPrimitive::unknown>(state, param);

            paramElement["type"] = paramTyping;

            paramElement["parameterName"] = param->getTypeName();
            parameters.push_back(paramElement);
        }

        functionDefinition["parameters"] = parameters;
        functionDefinition["returnType"] = returnType;
        functionDefinition["name"] = typing->getTypeName();

        typeDefinitionTable["functionDefinition"] = functionDefinition;

        return typeDefinitionTable;
    }

    template <>
    json makeTypingObjectFromTypeDefinition(sol::state &state, std::shared_ptr<TypeWrapper<DataPrimitive::object>> typing)
    {
        if (existingTypeMappings<DataPrimitive::object>.count(typing) > 0)
        {
            return existingTypeMappings<DataPrimitive::object>.at(typing);
        }

        auto asUnknown = std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>(new TypeWrapper<DataPrimitive::unknown>(typing));
        json typeDefinitionTable = makeTypingObjectFromTypeDefinition<DataPrimitive::unknown>(state, asUnknown);
        typeDefinitionTable["name"] = typing->getTypeName();
        existingTypeMappings<DataPrimitive::object>.insert_or_assign(typing, typeDefinitionTable);

        typeDefinitionTable["dataPrimitive"] = dataPrimitiveNameMap.at(DataPrimitive::object);

        json objectDefinition = json();
        json objectFields = json();

        for (std::pair<std::string, std::shared_ptr<APICore::TypeWrapper<APICore::unknown>>> param : typing->getFields())
        {
            json fieldTyping = makeTypingObjectFromTypeDefinition<DataPrimitive::unknown>(state, param.second);
            fieldTyping["name"] = param.first;
            objectFields[param.first] = fieldTyping;
        }

        auto instanceOf = typing->getInstanceOf();

        objectDefinition["objectFields"] = objectFields;
        if (instanceOf != nullptr)
        {
            objectDefinition["instanceOf"] = makeTypingObjectFromTypeDefinition<DataPrimitive::classType>(state, instanceOf);
        }
        typeDefinitionTable["objectDefinition"] = objectDefinition;

        return typeDefinitionTable;
    }

    template <>
    json makeTypingObjectFromTypeDefinition(sol::state &state, std::shared_ptr<TypeWrapper<DataPrimitive::array>> typing)
    {
        if (existingTypeMappings<DataPrimitive::array>.count(typing) > 0)
        {
            return existingTypeMappings<DataPrimitive::array>.at(typing);
        }

        auto asUnknown = std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>(new TypeWrapper<DataPrimitive::unknown>(typing));
        json typeDefinitionTable = makeTypingObjectFromTypeDefinition<DataPrimitive::unknown>(state, asUnknown);
        typeDefinitionTable["name"] = typing->getTypeName();
        existingTypeMappings<DataPrimitive::array>.insert_or_assign(typing, typeDefinitionTable);

        typeDefinitionTable["dataPrimitive"] = dataPrimitiveNameMap.at(DataPrimitive::array);

        // TODO: generate the "arrayOf" property
        // typeDefinitionTable["arrayOf"] = objectDefinition;

        return typeDefinitionTable;
    }

    template <>
    json makeTypingObjectFromTypeDefinition(sol::state &state, std::shared_ptr<ClassTypeWrapper> typing)
    {
        if (existingTypeMappings<DataPrimitive::classType>.count(typing) > 0)
        {
            return existingTypeMappings<DataPrimitive::classType>.at(typing);
        }

        auto asUnknown = std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>(new TypeWrapper<DataPrimitive::unknown>(typing));
        json typeDefinitionTable = makeTypingObjectFromTypeDefinition<DataPrimitive::unknown>(state, asUnknown);
        typeDefinitionTable["name"] = typing->getTypeName();
        existingTypeMappings<DataPrimitive::classType>.insert_or_assign(typing, typeDefinitionTable);

        typeDefinitionTable["dataPrimitive"] = dataPrimitiveNameMap.at(DataPrimitive::classType);

        auto classDefinition = json();

        auto staticTyping = typing->getStaticType();
        auto constructor = typing->getConstructor();

        classDefinition["classStaticType"] = makeTypingObjectFromTypeDefinition<DataPrimitive::object>(state, staticTyping);
        classDefinition["constructor"] = makeTypingObjectFromTypeDefinition<DataPrimitive::function>(state, constructor);
        classDefinition["constructor"]["functionDefinition"]["isConstructor"] = true;
        classDefinition["className"] = typing->getTypeName();

        typeDefinitionTable["classDefinition"] = classDefinition;

        return typeDefinitionTable;
    }

    template <DataPrimitive D>
    json makeTypingObjectFromDataWrapper(sol::state &state, std::shared_ptr<DataWrapperSub<D>> data)
    {
        json typeDefinitionTable;
        typeDefinitionTable["dataPrimitive"] = dataPrimitiveNameMap.at(data->getDataType());
        typeDefinitionTable["readonly"] = !data->canSet();

        return typeDefinitionTable;
    };

    template <>
    json makeTypingObjectFromDataWrapper(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::object>> data)
    {
        auto unknownWrapper = CastSharedPtr(DataWrapperSub<DataPrimitive::unknown>, data);
        json typeDefinitionTable = makeTypingObject<DataPrimitive::unknown>(state, unknownWrapper);

        json objectFields = json();

        data_primitive_to_type<DataPrimitive::object> objectFieldMap = *data->get();

        for (auto field : objectFieldMap)
        {

#define D(Primitive, Type)                                                                       \
    case DataPrimitive::Primitive:                                                               \
    {                                                                                            \
        auto castData = CastSharedPtr(DataWrapperSub<DataPrimitive::Primitive>, field.second);   \
        objectFields[field.first] = makeTypingObject<DataPrimitive::Primitive>(state, castData); \
        objectFields[field.first]["name"] = field.first;\
    };                                                                                           \
    break;

            switch (field.second->getDataType())
            {
                DATA_PRIMITIVE_VALUES;
            }

#undef D
        }

        json objectDefinition = json();
        objectDefinition["objectFields"] = objectFields;
        typeDefinitionTable["objectDefinition"] = objectDefinition;

        return typeDefinitionTable;
    };

    template <>
    json makeTypingObjectFromDataWrapper(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::classType>> data)
    {
        auto unknownWrapper = CastSharedPtr(DataWrapperSub<DataPrimitive::unknown>, data);
        json typeDefinitionTable = makeTypingObject<DataPrimitive::unknown>(state, unknownWrapper);

        json classDefinition = json();
        auto classData = data->get();

        classDefinition["className"] = classData->className;

        auto constructorTyping = makeTypingObject<DataPrimitive::function>(state, classData->constructor);
        classDefinition["constructor"] = constructorTyping;
        classDefinition["constructor"]["functionDefinition"]["isConstructor"] = true;

        classDefinition["classInstanceType"] = constructorTyping["functionDefinition"]["returnType"];

        typeDefinitionTable["classDefinition"] = classDefinition;
        return typeDefinitionTable;
    }

    std::string generateTypings(std::string key, sol::state &state, std::map<std::string, std::shared_ptr<DataWrapper>> entries)
    {
        state["_handler"] = ([](sol::variadic_args e)
                             { 
            std::cout << "Error occurred. ";
            for(auto x : e){
                (std::cout << "\t") << x.as<std::string>();
            }
                std::cout << "\n";
            return -1; });

        std::vector<json> typeList;

        for (std::pair<std::string, std::shared_ptr<APICore::DataWrapper>> entry : entries)
        {
            json typing;

#define D(Primitive, Type)                                                                     \
    case DataPrimitive::Primitive:                                                             \
    {                                                                                          \
        auto castData = CastSharedPtr(DataWrapperSub<DataPrimitive::Primitive>, entry.second); \
        typing = makeTypingObject<DataPrimitive::Primitive>(state, castData);                  \
    };                                                                                         \
    break;

            switch (entry.second->getDataType())
            {
                DATA_PRIMITIVE_VALUES;
            }

#undef D

            typing["name"] = entry.first;
            typeList.push_back(typing);
        }

        std::string result = json(typeList).dump();

#define D(Primitive, Type) \
    existingTypeMappings<DataPrimitive::Primitive>.clear();

        DATA_PRIMITIVE_VALUES

#undef D

        return result;
    }
}

#endif