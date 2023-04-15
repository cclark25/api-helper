#pragma once

#include "./string-literal.hpp"
#include "./static-pointer.hpp"
#include "./type-lookup.hpp"
#include "./function-parameters.hpp"
#include <type_traits>
#include <string>
#include <vector>

namespace APICore
{
    template <StringLiteral Key, auto Pointer, StringLiteral Description, ParameterPackDefinition Parameters>
    struct StaticFunction : public Static<Key, Pointer, Description>
    {
        using pointerTypeLookup = void;
        using parameterPack = Parameters;
        static const bool isConstructor = false;
    };

    template <auto Pointer, StringLiteral Description, ParameterPackDefinition Parameters>
    struct Constructor : public StaticFunction<"__constructor", Pointer, Description, Parameters>
    {
        static const bool isConstructor = true;
    };

    template <typename PointerType>
    struct StaticFunctionTyping
    {
        using returnType = void;

        template <class T>
        using childMemberCast = PointerType;
    };

    template <typename ReturnType, typename... Parameters>
    struct StaticFunctionTyping<ReturnType *(Parameters...)>
    {
        using returnType = ReturnType;

        template <class T>
        using childMemberCast = ReturnType *(Parameters...);
    };

    template <typename PointerType, StringLiteral Key, PointerType Pointer, StringLiteral Description, ParameterPackDefinition Parameters>
    struct StaticFunction<Key, Pointer, Description, Parameters> : public Static<Key, Pointer, Description>
    {
        using functionTyping = StaticFunctionTyping<PointerType>;
        using parameterPack = Parameters;
        static bool lookupCreated;
        static const bool isConstructor = false;
    };

    template<class T>
    concept ConstructorSpec = requires {
        requires T::isConstructor;
    };

    

    template <class T>
    concept StaticOverloadSpec = requires() {
        requires T::isStaticOverload;
    };

    template <StaticFunctionPtrSpec FirstFunction, StaticFunctionPtrSpec... Functions>
    struct StaticOverload
    {
        static const bool isStaticOverload = true;
        static const bool isConstructor = FirstFunction::isConstructor;
        static std::string key;
        static size_t functionCount;
        static std::string getKey()
        {
            return FirstFunction::key;
        }

        template <StaticFunctionPtrSpec... OtherFunctions>
        using AddOverloads = StaticOverload<FirstFunction, OtherFunctions..., Functions...>;

        template<template<class...> class Temp>
        using IterateOverloads = Temp<FirstFunction, Functions...>;
    };
    template <StaticFunctionPtrSpec FirstFunction, StaticFunctionPtrSpec... Functions>
    std::string StaticOverload<FirstFunction, Functions...>::key = FirstFunction::key;
    template <StaticFunctionPtrSpec FirstFunction, StaticFunctionPtrSpec... Functions>
    size_t StaticOverload<FirstFunction, Functions...>::functionCount = 1 + sizeof...(Functions);
};
