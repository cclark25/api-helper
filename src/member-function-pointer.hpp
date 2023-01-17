#ifndef MEMBER_FUNCTION_POINTER
#define MEMBER_FUNCTION_POINTER

#include "./string-literal.hpp"
#include "./member-pointer.hpp"
#include "./type-lookup.hpp"
#include "./function-parameters.hpp"
#include <type_traits>
#include <string>
#include <vector>
#include <functional>

namespace APICore
{
    template <StringLiteral Key, auto Pointer, StringLiteral Description, ParameterPackDefinition Parameters>
    struct MemberFunction : public MemberPtr<Pointer>
    {
        using pointerTypeLookup = void;
        using parameterPack = Parameters;
        static std::string key;
        static std::string description;
    };
    template <StringLiteral Key, auto Pointer, StringLiteral Description, ParameterPackDefinition Parameters>
    std::string MemberFunction<Key, Pointer, Description, Parameters>::key = Key.value;
    template <StringLiteral Key, auto Pointer, StringLiteral Description, ParameterPackDefinition Parameters>
    std::string MemberFunction<Key, Pointer, Description, Parameters>::description = Description.value;

    template <typename PointerType>
    struct MemberFunctionTyping
    {
        using classType = void;
        using returnType = void;
    };

    template <class FunctionType, typename ReturnType, typename... Parameters>
    struct MemberFunctionTyping<ReturnType FunctionType::*(Parameters...)>
    {
        using classType = FunctionType;
        using returnType = ReturnType;

      
    };

    template <typename PointerType, StringLiteral Key, PointerType Pointer, StringLiteral Description, ParameterPackDefinition Parameters>
    struct MemberFunction<Key, Pointer, Description, Parameters> : public MemberPtr<Pointer>
    {
        using functionTyping = MemberFunctionTyping<PointerType>;
        using parameterPack = Parameters;
        static std::vector<std::pair<std::string, std::string>> parameters;
        static std::string key;
        static std::string description;
        static bool lookupCreated;
    };

    template <typename PointerType, StringLiteral Key, PointerType Pointer, StringLiteral Description, ParameterPackDefinition Parameters>
    std::string MemberFunction<Key, Pointer, Description, Parameters>::key = Key.value;
    template <typename PointerType, StringLiteral Key, PointerType Pointer, StringLiteral Description, ParameterPackDefinition Parameters>
    std::string MemberFunction<Key, Pointer, Description, Parameters>::description = Description.value;

    template <typename T, typename ClassToBind>
    concept MemberLikeFunction = requires(T v, ClassToBind &c) {
        { std::bind(v, c) };
    };

    template <typename T>
    concept MemberFunctionPtrSpec = requires()
    {
        requires MemberPtrSpec<T> ;
        {T::functionTyping::classType};
        {T::functionTyping::returnType};
    };
};

#endif