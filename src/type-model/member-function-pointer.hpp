#ifndef MEMBER_FUNCTION_POINTER
#define MEMBER_FUNCTION_POINTER

#include "./string-literal.hpp"
#include "./member-pointer.hpp"
#include "./type-lookup.hpp"
#include <type_traits>
#include <string>
#include <vector>

namespace APICore
{
    template <StringLiteral Name, StringLiteral Description>
    struct Parameter
    {
        static std::string name;
        static std::string description;
    };
    template <StringLiteral Name, StringLiteral Description>
    std::string Parameter<Name, Description>::name = Name;
    template <StringLiteral Name, StringLiteral Description>
    std::string Parameter<Name, Description>::description = Description;

    template <typename T>
    concept ParameterDescription = requires
    {
        {
            T::name
            } -> std::convertible_to<std::string>;
        {
            T::description
            } -> std::convertible_to<std::string>;
    };

    template <StringLiteral Key, auto Pointer, StringLiteral Description, ParameterDescription... Parameters>
    struct MemberFunction : public MemberPtr<Pointer>
    {
        using pointerTypeLookup = void;
        static std::vector<std::pair<std::string, std::string>> parameters;
        static std::string key;
        static std::string description;
    };
    template <StringLiteral Key, auto Pointer, StringLiteral Description, ParameterDescription... Parameters>
    std::vector<std::pair<std::string, std::string>>
        MemberFunction<Key, Pointer, Description, Parameters...>::parameters = std::vector<std::pair<std::string, std::string>>((std::pair<std::string, std::string>(Parameters::name, Parameters::description), ...));
    template <StringLiteral Key, auto Pointer, StringLiteral Description, ParameterDescription... Parameters>
    std::string MemberFunction<Key, Pointer, Description, Parameters...>::key = Key.value;
    template <StringLiteral Key, auto Pointer, StringLiteral Description, ParameterDescription... Parameters>
    std::string MemberFunction<Key, Pointer, Description, Parameters...>::description = Description.value;

    template <typename PointerType>
    struct MemberFunctionTyping
    {
        using classType = void;
        using returnType = void;
    };
    template <class ClassType, typename ReturnType, typename... Parameters>
    struct MemberFunctionTyping<ReturnType ClassType::*(Parameters...)>
    {
        using classType = ClassType;
        using returnType = ReturnType;
    };

    template <typename PointerType, StringLiteral Key, PointerType Pointer, StringLiteral Description, ParameterDescription... Parameters>
    struct MemberFunction<Key, Pointer, Description, Parameters...> : public MemberPtr<Pointer>
    {
        using functionTyping = MemberFunctionTyping<PointerType>;
        using pointerTypeLookup = ValueTypeLookup<PointerType, Pointer>;
        static std::vector<std::pair<std::string, std::string>> parameters;
        static std::string key;
        static std::string description;
    };
    template <typename PointerType, StringLiteral Key, PointerType Pointer, StringLiteral Description, ParameterDescription... Parameters>
    std::vector<std::pair<std::string, std::string>>
        MemberFunction<Key, Pointer, Description, Parameters...>::parameters = std::vector<std::pair<std::string, std::string>>((std::pair<std::string, std::string>(Parameters::name, Parameters::description), ...));
    template <typename PointerType, StringLiteral Key, PointerType Pointer, StringLiteral Description, ParameterDescription... Parameters>
    std::string MemberFunction<Key, Pointer, Description, Parameters...>::key = Key.value;
    template <typename PointerType, StringLiteral Key, PointerType Pointer, StringLiteral Description, ParameterDescription... Parameters>
    std::string MemberFunction<Key, Pointer, Description, Parameters...>::description = Description.value;

    template <typename T>
    concept MemberFunctionPtrSpec = requires()
    {
        requires MemberPtrSpec<T>;
        {
            T::parameters
            } -> std::convertible_to<std::vector<std::pair<std::string, std::string>>>;
        {T::functionTyping::classType};
        {T::functionTyping::returnType};
    };
};

#endif