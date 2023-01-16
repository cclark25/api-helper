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

    template <class FunctionType, typename T, T FunctionType::*Pointer>
    struct MemberPtr<Pointer>
    {
        using classType = FunctionType;
        using type = T;
        using ptrType = type classType::*;
        static ptrType ptr;
    };
    template <class FunctionType, typename T, T FunctionType::*Pointer>
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

};

#endif