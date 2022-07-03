
#include <sol/sol.hpp>
#include "../../data-wrappers/data-wrapper.hpp"
#include "./class-type.h"
#include "./lua-object-to-data-wrapper.h"
#include "./create-binding-object.h"
#include "../../macros.hpp"
#include "../lua-loader.hpp"

#define LUA_SOURCE_FUNCTION_KEY "API_LUA"

namespace APILua
{
    using namespace APICore;

    template <>
    sol::table createBindingObject<DataPrimitive::classType>(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::classType>> wrapper)
    {
        sol::table table = sol::table(state, sol::new_table());

        if (wrapper->canGet())
        {
            APICore::Data<APICore::classType> classPtr = wrapper->get();
            auto x = 0;
            table["get"] = [wrapper, &state]()
            {
                APICore::Data<APICore::classType> classPtr = wrapper->get();

                if (classPtr->externalSource != nullptr && classPtr->externalSourceKeyName == LUA_SOURCE_FUNCTION_KEY)
                {
                    auto src = classPtr->externalSource->classDefinition;
                    auto result = CastSharedPtr(sol::table, src);
                    std::string name = (*result)["name"];
                    return *result;
                }

                auto x = 0;

                std::function<sol::table(sol::variadic_args)> wrappedConstructor = [classPtr, &state](sol::variadic_args arguments)
                {
                    if (classPtr == nullptr)
                    {
                        throw sol::error("classPtr is a null pointer!\n");
                    }
                    FunctionInternalType::FunctionParams wrappedParams;
                    for (auto param : arguments)
                    {
                        std::shared_ptr<DataWrapper> paramWrapper = luaObjectToDataWrapper(param.as<sol::object>());
                        if (paramWrapper == nullptr)
                        {
                            continue;
                        }
                        wrappedParams.push_back(paramWrapper);
                        continue;
                    }
                    auto x = &classPtr->constructor;
                    auto constructorResults = classPtr->constructor(wrappedParams);

                    sol::table resultTable = sol::table(state, sol::new_table());

                    for (auto field : constructorResults)
                    {
                        auto value = CastSharedPtr(DataWrapperSub<DataPrimitive::unknown>, field.second);
                        resultTable[field.first] = createBindingObject<DataPrimitive::unknown>(state, value);
                    }

                    return resultTable;
                };

                auto makeClass = sol::state_view(state)["____bindingHelpers"]["makeClass"];

                if (!makeClass.valid())
                {
                    state.add_package_loader(LoadFile);
                    state.script("____bindingHelpers = require('binding-helpers');");
                    makeClass = sol::state_view(state)["____bindingHelpers"]["makeClass"];
                }

                sol::table staticFields(state, sol::new_table());

                for (auto field : classPtr->staticFields)
                {
                    sol::table bindingObject = createBindingObject(state, CastSharedPtr(DataWrapperSub<DataPrimitive::unknown>, field.second));
                    staticFields[field.first] = bindingObject;
                }

                sol::table generatedClassObject = makeClass(nullptr, classPtr->className, wrappedConstructor, staticFields);

                sol::table generatedClass = generatedClassObject["generatedClass"];
                auto classTable = std::shared_ptr<sol::table>(new sol::table(state, sol::new_table()));
                *(classTable) = generatedClass;

                std::shared_ptr<ClassInternalType::ExternalSource> definition(new ClassInternalType::ExternalSource({classTable,
                                                                                                                     [generatedClassObject, &state](std::shared_ptr<ObjectMap> data)
                                                                                                                     {
                                                                                                                         sol::function bindAs = generatedClassObject["bindAs"];

                                                                                                                         sol::table memberFields(state, sol::new_table());

                                                                                                                         for (auto param : *data)
                                                                                                                         {
                                                                                                                             auto p = CastSharedPtr(DataWrapperSub<DataPrimitive::unknown>, param.second);
                                                                                                                             auto bounded = createBindingObject(state, p);
                                                                                                                             memberFields[param.first] = bounded;
                                                                                                                         }

                                                                                                                         return std::shared_ptr<sol::table>(new sol::table((sol::table)bindAs(nullptr, memberFields)));
                                                                                                                     }}));

                classPtr->externalSource = definition;
                classPtr->externalSourceKeyName = LUA_SOURCE_FUNCTION_KEY;

                return generatedClass;
            };
        }
        if (wrapper->canSet())
        {
        }

        return table;
    }

    template <>
    sol::table createBindingObject<DataPrimitive::classInstance>(sol::state &state, std::shared_ptr<DataWrapperSub<DataPrimitive::classInstance>> wrapper)
    {
        sol::table table = sol::table(state, sol::new_table());

        if (wrapper->canGet() && wrapper->get()->data->canGet())
        {
            table["get"] = [wrapper, &state]()
            {
                auto instanceData = wrapper->get();
                auto classReference = instanceData->classDef->get();
                if (classReference->externalSourceKeyName != LUA_SOURCE_FUNCTION_KEY || classReference->externalSource == nullptr)
                {
                    createBindingObject<DataPrimitive::classType>(state, instanceData->classDef)["get"]();
                }

                auto data = instanceData->data->get();

                auto bound = classReference->externalSource->bindAs(data);

                return *CastSharedPtr(sol::table, bound);
            };
        }

        return table;
    }
}