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
    sol::table makeTypingObject(sol::state &state, std::shared_ptr<DataWrapperSub<D>> data)
    {
        auto typeDefinitionTable = sol::table(state, sol::new_table());
        typeDefinitionTable["dataPrimitive"] = dataPrimitiveNameMap.at(data->getDataType());

        return typeDefinitionTable;
    };

    template <>
    sol::table makeTypingObject(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::object>> data)
    {
        auto unknownWrapper = CastSharedPtr(DataWrapperSub<DataPrimitive::unknown>, data);
        sol::table typeDefinitionTable = makeTypingObject<DataPrimitive::unknown>(state, unknownWrapper);

        sol::table objectFields = sol::table(state, sol::new_table());

        for (auto field : *data->get())
        {

#define D(Primitive, Type)                                                                           \
    case DataPrimitive::Primitive:                                                                   \
    {                                                                                                \
        auto castData = CastSharedPtr(DataWrapperSub<DataPrimitive::Primitive>, field.second);       \
        objectFields[field.first] = makeTypingObject<DataPrimitive::Primitive>(state, castData); \
    };                                                                                               \
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
        auto declare = typeGeneration["declare"];
        auto generateTypeFiles = typeGeneration["generateTypeFiles"];

        sol::table typeList = sol::table(state, sol::new_table());
        size_t typeListLength = 0;

        for (std::pair<std::string, std::shared_ptr<APICore::DataWrapper>> entry : entries)
        {
            sol::table typing;

#define D(Primitive, Type)                                                                     \
    case DataPrimitive::Primitive: {                                                            \
        auto castData = CastSharedPtr(DataWrapperSub<DataPrimitive::Primitive>, entry.second); \
        typing = makeTypingObject<DataPrimitive::Primitive>(state, castData);              \
    }; \
        break;

            switch (entry.second->getDataType())
            {
                DATA_PRIMITIVE_VALUES;
            }

#undef D

            typeList[typeListLength++] = declare(nullptr, entry.first, typing);
        }

        std::string result = generateTypeFiles( nullptr, key, typeList);

        return result;
    }
}

#endif