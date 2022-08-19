#ifndef LUA_TYPE_GENERATION
#define LUA_TYPE_GENERATION

#include <map>
#include <memory>
#include <sol.hpp>
#include "../data-wrappers/data-wrapper.hpp"
#include "./binding-types/create-binding-object.h"
#include "./lua-loader.hpp"

namespace APILua
{
    using namespace APICore;

    template <DataPrimitive D>
    sol::table makeTypingObjectFromTypeDefinition(sol::state &state, std::shared_ptr<TypeWrapper<D>> typing);
    template <>
    sol::table makeTypingObjectFromTypeDefinition(sol::state &state, std::shared_ptr<TypeWrapper<DataPrimitive::function>> typing);
    template <>
    sol::table makeTypingObjectFromTypeDefinition(sol::state &state, std::shared_ptr<TypeWrapper<DataPrimitive::object>> typing);
    template <>
    sol::table makeTypingObjectFromTypeDefinition(sol::state &state, std::shared_ptr<TypeWrapper<DataPrimitive::classType>> typing);

    template <DataPrimitive D>
    sol::table makeTypingObjectFromDataWrapper(sol::state &state, std::shared_ptr<DataWrapperSub<D>> data);
    template <>
    sol::table makeTypingObjectFromDataWrapper(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::object>> data);
    template <>
    sol::table makeTypingObjectFromDataWrapper(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::classType>> data);

    template <DataPrimitive D>
    sol::table makeTypingObject(sol::state &state, std::shared_ptr<DataWrapperSub<D>> data)
    {
        auto typingPtr = data->getType();
        if (typingPtr == nullptr)
        {
            return makeTypingObjectFromDataWrapper<D>(state, data);
        }
        else
        {
            auto castPtr = CastSharedPtr(TypeWrapper<D>, typingPtr);
            return makeTypingObjectFromTypeDefinition<D>(state, castPtr);
        }
    }

    template <DataPrimitive D>
    sol::table makeTypingObjectFromTypeDefinition(sol::state &state, std::shared_ptr<TypeWrapper<D>> typing)
    {
        auto typeDefinitionTable = sol::table(state, sol::new_table());
        DataPrimitive prim = typing->getPrimitiveType();
        typeDefinitionTable["dataPrimitive"] = dataPrimitiveNameMap.at(prim);

        return typeDefinitionTable;
    }

    template <>
    sol::table makeTypingObjectFromTypeDefinition<DataPrimitive::unknown>(sol::state &state, std::shared_ptr<TypeWrapper<DataPrimitive::unknown>> typing)
    {
        DataPrimitive prim = typing == nullptr ? DataPrimitive::unknown : typing->getPrimitiveType();

        if (prim == DataPrimitive::unknown)
        {
            auto typeDefinitionTable = sol::table(state, sol::new_table());
            typeDefinitionTable["dataPrimitive"] = dataPrimitiveNameMap.at(prim);

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
    sol::table makeTypingObjectFromTypeDefinition(sol::state &state, std::shared_ptr<TypeWrapper<DataPrimitive::function>> typing)
    {
        auto asUnknown = std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>(new TypeWrapper<DataPrimitive::unknown>(typing));
        sol::table typeDefinitionTable = makeTypingObjectFromTypeDefinition<DataPrimitive::unknown>(state, asUnknown);
        typeDefinitionTable["dataPrimitive"] = dataPrimitiveNameMap.at(DataPrimitive::function);

        sol::table functionDefinition = sol::table(state, sol::new_table());
        sol::table parameters = sol::table(state, sol::new_table());
        sol::table returnType = makeTypingObjectFromTypeDefinition<DataPrimitive::unknown>(state, typing->getReturnType());

        size_t parameterIndex = 1;
        for (auto param : typing->getParams())
        {
            sol::table paramElement = sol::table(state, sol::new_table());
            sol::table paramTyping = makeTypingObjectFromTypeDefinition<DataPrimitive::unknown>(state, param);

            paramElement["type"] = paramTyping;

            paramElement["parameterName"] = param->getTypeName();
            parameters[parameterIndex] = paramElement;

            parameterIndex++;
        }

        functionDefinition["parameters"] = parameters;
        functionDefinition["returnType"] = returnType;

        typeDefinitionTable["functionDefinition"] = functionDefinition;

        return typeDefinitionTable;
    }

    template <>
    sol::table makeTypingObjectFromTypeDefinition(sol::state &state, std::shared_ptr<TypeWrapper<DataPrimitive::object>> typing)
    {
        auto asUnknown = std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>(new TypeWrapper<DataPrimitive::unknown>(typing));
        sol::table typeDefinitionTable = makeTypingObjectFromTypeDefinition<DataPrimitive::unknown>(state, asUnknown);
        typeDefinitionTable["dataPrimitive"] = dataPrimitiveNameMap.at(DataPrimitive::object);

        sol::table objectFields = sol::table(state, sol::new_table());

        for (std::pair<std::string, std::shared_ptr<APICore::TypeWrapper<APICore::unknown>>> param : typing->getFields())
        {
            sol::table fieldTyping = makeTypingObjectFromTypeDefinition<DataPrimitive::unknown>(state, param.second);
            objectFields[param.first] = fieldTyping;
        }

        typeDefinitionTable["objectFields"] = objectFields;

        return typeDefinitionTable;
    }

    template <>
    sol::table makeTypingObjectFromTypeDefinition(sol::state &state, std::shared_ptr<ClassTypeWrapper> typing)
    {
        auto asUnknown = std::shared_ptr<TypeWrapper<DataPrimitive::unknown>>(new TypeWrapper<DataPrimitive::unknown>(typing));
        sol::table typeDefinitionTable = makeTypingObjectFromTypeDefinition<DataPrimitive::unknown>(state, asUnknown);
        typeDefinitionTable["dataPrimitive"] = dataPrimitiveNameMap.at(DataPrimitive::classType);

        auto classDefinition = sol::table(state, sol::new_table());

        auto staticTyping = typing->getStaticType();
        auto instanceTyping = typing->getInstanceType();
        auto constructor = typing->getConstructor();

        classDefinition["classStaticType"] = makeTypingObjectFromTypeDefinition<DataPrimitive::object>(state, staticTyping);
        classDefinition["constructor"] = makeTypingObjectFromTypeDefinition<DataPrimitive::function>(state, constructor);
        classDefinition["classInstanceType"] = classDefinition["constructor"]["functionDefinition"]["returnType"];
        classDefinition["constructor"]["functionDefinition"]["isConstructor"] = true;
        classDefinition["className"] = typing->getTypeName();

        typeDefinitionTable["classDefinition"] = classDefinition;

        return typeDefinitionTable;
    }

    template <DataPrimitive D>
    sol::table makeTypingObjectFromDataWrapper(sol::state &state, std::shared_ptr<DataWrapperSub<D>> data)
    {
        auto typeDefinitionTable = sol::table(state, sol::new_table());
        typeDefinitionTable["dataPrimitive"] = dataPrimitiveNameMap.at(data->getDataType());

        return typeDefinitionTable;
    };

    template <>
    sol::table makeTypingObjectFromDataWrapper(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::object>> data)
    {
        auto unknownWrapper = CastSharedPtr(DataWrapperSub<DataPrimitive::unknown>, data);
        sol::table typeDefinitionTable = makeTypingObject<DataPrimitive::unknown>(state, unknownWrapper);

        sol::table objectFields = sol::table(state, sol::new_table());

        data_primitive_to_type<DataPrimitive::object> objectFieldMap = *data->get();

        for (auto field : objectFieldMap)
        {

#define D(Primitive, Type)                                                                       \
    case DataPrimitive::Primitive:                                                               \
    {                                                                                            \
        auto castData = CastSharedPtr(DataWrapperSub<DataPrimitive::Primitive>, field.second);   \
        objectFields[field.first] = makeTypingObject<DataPrimitive::Primitive>(state, castData); \
    };                                                                                           \
    break;

            switch (field.second->getDataType())
            {
                DATA_PRIMITIVE_VALUES;
            }

#undef D
        }

        typeDefinitionTable["objectFields"] = objectFields;

        return typeDefinitionTable;
    };

    template <>
    sol::table makeTypingObjectFromDataWrapper(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::classType>> data)
    {
        auto unknownWrapper = CastSharedPtr(DataWrapperSub<DataPrimitive::unknown>, data);
        sol::table typeDefinitionTable = makeTypingObject<DataPrimitive::unknown>(state, unknownWrapper);

        sol::table classDefinition = sol::table(state, sol::new_table());
        auto classData = data->get();

        classDefinition["className"] = classData->className;

        auto constructorTyping = makeTypingObject<DataPrimitive::function>(state, classData->constructor);
        classDefinition["constructor"] = constructorTyping;
        classDefinition["constructor"]["functionDefinition"]["isConstructor"] = true;

        classDefinition["classInstanceType"] = constructorTyping["functionDefinition"]["returnType"];
        // classDefinition["classStaticType"] = ;

        typeDefinitionTable["classDefinition"] = classDefinition;
        return typeDefinitionTable;
    }

    std::string generateTypings(std::string key, sol::state &state, std::map<std::string, std::shared_ptr<DataWrapper>> entries)
    {
        auto typeGeneration = sol::state_view(state)["____typeGeneration"]["TypeGeneration"];
        if (!typeGeneration.valid())
        {
            state.add_package_loader(LoadFile);
            state.script("____typeGeneration = require('type-generation');");
            typeGeneration = sol::state_view(state)["____typeGeneration"]["default"];
        }

        auto generateType = typeGeneration["generateType"];
        sol::function declare = typeGeneration["declare"];
        auto generateTypeFiles = typeGeneration["generateTypeFiles"];

        state["_handler"] = ([](sol::variadic_args e)
                             { 
            std::cout << "Error occurred. ";
            for(auto x : e){
                (std::cout << "\t") << x.as<std::string>();
            }
                std::cout << "\n";
            return -1; });

        sol::table typeList = sol::table(state, sol::new_table());

        declare.set_error_handler(state["_handler"]);
        size_t typeListLength = 1;

        for (std::pair<std::string, std::shared_ptr<APICore::DataWrapper>> entry : entries)
        {
            sol::table typing;

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

            auto declared = declare(nullptr, entry.first, typing);
            auto luaType = declared.get_type();
            if (luaType != sol::type::lua_nil)
            {
                typeList[typeListLength++] = declared;
            }
        }

        std::string result = generateTypeFiles(nullptr, key, typeList);

        return result;
    }
}

#endif