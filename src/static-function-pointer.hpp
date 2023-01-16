#ifndef STATIC_FUNCTION_POINTER
#define STATIC_FUNCTION_POINTER

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
    };

    template <typename PointerType>
    struct StaticFunctionTyping
    {
        using returnType = void;
    };

    template <typename ReturnType, typename... Parameters>
    struct StaticFunctionTyping<ReturnType *(Parameters...)>
    {
        using returnType = ReturnType;      
    };

    template <typename PointerType, StringLiteral Key, PointerType Pointer, StringLiteral Description, ParameterPackDefinition Parameters>
    struct StaticFunction<Key, Pointer, Description, Parameters> : public Static<Key, Pointer, Description>
    {
        using functionTyping = StaticFunctionTyping<PointerType>;
        using parameterPack = Parameters;
        static bool lookupCreated;
    };

    template <typename T>
    concept StaticFunctionPtrSpec = requires()
    {
        requires StaticPtrSpec<T>;
        {T::functionTyping::returnType};
    };
};

#endif