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
    sol::table makeTypingObjectFromTypeDefinition(sol::state &state, std::shared_ptr<TypeWrapper<DataPrimitive::function>> typing)
    {
        auto castAsUnknown = CastSharedPtr(TypeWrapper<DataPrimitive::unknown>, typing);
        sol::table typeDefinitionTable = makeTypingObjectFromTypeDefinition<DataPrimitive::unknown>(state, castAsUnknown);

        // TODO: add fields for function typing.
        assasfasf

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

        classDefinition["className"] = data->get()->className;
        // TODO: add type generation for class instance and constructor fields.

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