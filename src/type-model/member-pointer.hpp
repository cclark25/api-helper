#ifndef MEMBER_POINTER
#define MEMBER_POINTER

#include "./string-literal.hpp"
#include <type_traits>
#include <string>

namespace APICore
{
    template <auto Pointer>
    struct MemberPtr
    {
        using classType = void;
        using type = void;
        using ptrType = void *;
        static ptrType ptr;
    };
    template <auto Pointer>
    MemberPtr<Pointer>::ptrType MemberPtr<Pointer>::ptr = nullptr;

    template <class ClassType, typename T, T ClassType::*Pointer>
    struct MemberPtr<Pointer>
    {
        using classType = ClassType;
        using type = T;
        using ptrType = type classType::*;
        static ptrType ptr;
    };
    template <class ClassType, typename T, T ClassType::*Pointer>
    MemberPtr<Pointer>::ptrType MemberPtr<Pointer>::ptr = Pointer;

    template <StringLiteral Key, auto Pointer, StringLiteral Description = "">
    struct Member : public MemberPtr<Pointer>
    {
        static std::string key;
        static std::string description;
    };
    template <StringLiteral Key, auto Pointer, StringLiteral Description>
    std::string Member<Key, Pointer, Description>::key = Key.value;
    template <StringLiteral Key, auto Pointer, StringLiteral Description>
    std::string Member<Key, Pointer, Description>::description = Description.value;

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

    // template <StringLiteral Key, auto Pointer, StringLiteral Description, ParameterDescription... Parameters>
    struct MemberFunction : public Member<Key, Pointer, Description>
    {
        static std::vector<std::pair<std::string, std::string>> parameters;
    };
    template <StringLiteral Key, auto Pointer, StringLiteral Description, ParameterDescription... Parameters>
    std::vector<std::pair<std::string, std::string>>
        MemberFunction<Key, Pointer, Description, Parameters...>::parameters = std::vector<std::pair<std::string, std::string>>((std::pair<std::string, std::string>(Parameters::name, Parameters::description), ...));

    template <typename T>
    concept MemberPtrSpec = requires(T::type typeVal, T::classType classVal, T::ptrType ptrVal)
    {
        {
            T::key
            } -> std::convertible_to<std::string>;
        {
            T::ptr
            } -> std::convertible_to<typename T::ptrType>;
        {
            T::ptr
            } -> std::convertible_to<typename T::type T::classType::*>;
    };

    template <typename T>
    concept MemberFunctionPtrSpec = requires()
    {
        requires MemberPtrSpec<T>;
    };
};

#endif