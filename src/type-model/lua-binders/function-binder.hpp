#ifndef FUNCTION_LUA_BINDER
#define FUNCTION_LUA_BINDER

#include "../type-lookup.hpp"
#include "../member-function-pointer.hpp"
#include "./type-binder.hpp"
#include <iostream>
#include <sol.hpp>

namespace APICore
{
    template <class ParentClassType, class MemberFunctionDef, typename ReturnType, typename... Parameters>
    struct LuaBinder<ParentClassType, ReturnType(Parameters...), "MEMBER", MemberFunctionDef>
    {
        static void bind(sol::state &state, sol::usertype<ParentClassType> * userType, MemberFunctionDef* memberPtr = nullptr)
        {
            // auto functionTyping = std::shared_ptr<
            //     TypeWrapper<
            //         DataPrimitive::function>>(
            //     new TypeWrapper<DataPrimitive::function>(
            //         "",
            //         "",
            //         std::vector<std::shared_ptr<APICore::TypeWrapper<APICore::unknown>>>(),
            //         makeBasicType(DataPrimitive::unknown)));

            // auto returnType = LuaBinder<ReturnType>::void(state);

            // functionTyping->setReturnType(returnType);

            // ((functionTyping->parameters.push_back(LuaBinder<Parameters>::bind(state))), ...);

            // size_t index = 0;
            // for(auto p : MemberFunctionDef::parameterPack::parameters){
            //     functionTyping->parameters[index]->name = p.first;
            //     functionTyping->parameters[index]->description = p.second;
            //     index++;
            // }

            return;
        }
    };

    template <class ParentClassType, class StaticFunctionDef, typename ReturnType, typename... Parameters>
    struct LuaBinder<ParentClassType, ReturnType(Parameters...), "STATIC", StaticFunctionDef>
    {
        static void bind(sol::state &state, sol::usertype<ParentClassType> * userType)
        {
            // auto functionTyping = std::shared_ptr<
            //     TypeWrapper<
            //         DataPrimitive::function>>(
            //     new TypeWrapper<DataPrimitive::function>(
            //         "",
            //         "",
            //         std::vector<std::shared_ptr<APICore::TypeWrapper<APICore::unknown>>>(),
            //         makeBasicType(DataPrimitive::unknown)));

            // auto returnType = LuaBinder<ReturnType>::generateTyping();

            // functionTyping->setReturnType(returnType);

            // ((functionTyping->parameters.push_back(LuaBinder<Parameters>::generateTyping())), ...);

            // size_t index = 0;
            // for(auto p : StaticFunctionDef::parameterPack::parameters){
            //     functionTyping->parameters[index]->name = p.first;
            //     functionTyping->parameters[index]->description = p.second;
            //     index++;
            // }

            return ;
        }
    };

    template <class ParentClassType, typename T, typename... ExtraData>
    struct LuaBinder<ParentClassType, T, "MEMBER", ExtraData...> : public LuaBinder<ParentClassType, T, "ANY", ExtraData...>
    {
    };
    template <class ParentClassType, typename T, typename... ExtraData>
    struct LuaBinder<ParentClassType, T, "STATIC", ExtraData...> : public LuaBinder<ParentClassType, T, "ANY", ExtraData...>
    {
    };

}

#endif