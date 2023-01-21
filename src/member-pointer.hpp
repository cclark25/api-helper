#ifndef MEMBER_POINTER
#define MEMBER_POINTER

#include "./string-literal.hpp"
#include <type_traits>
#include <string>
#include <iostream>

namespace APICore
{
    template <auto Pointer>
    struct MemberPtr
    {
        using classType = void;
        using type = void;
        using ptrType = void *;
        static ptrType ptr;
        static const bool isMember = true;
        static const bool isCustomMember = false;
    };
    template <auto Pointer>
    MemberPtr<Pointer>::ptrType MemberPtr<Pointer>::ptr = nullptr;

    template <class FunctionType, typename T, T FunctionType::*Pointer>
    requires std::is_member_pointer_v<decltype(Pointer)>
    struct MemberPtr<Pointer>
    {
        using classType = FunctionType;
        using type = T;
        using ptrType = type classType::*;
        static ptrType ptr;
        static const bool isMember = true;
        static const bool isCustomMember = false;
    };
    template <class FunctionType, typename T, T FunctionType::*Pointer>
    requires std::is_member_pointer_v<decltype(Pointer)>
    MemberPtr<Pointer>::ptrType MemberPtr<Pointer>::ptr = Pointer;

    template <class ClassType, typename ReturnType, class... Parameters, ReturnType (*Pointer)(ClassType&, Parameters...) >
    requires (!std::is_member_pointer_v<decltype(Pointer)> )
    struct MemberPtr<Pointer>
    {
        using classType = ClassType;
        using type = ReturnType(ClassType&, Parameters...);
        using ptrType = ReturnType (*) (ClassType&, Parameters...);
        static ptrType ptr;
        static const bool isMember = true;
        static const bool isCustomMember = true;
    };
    template <class ClassType, typename ReturnType, class... Parameters, ReturnType (*Pointer)(ClassType&, Parameters...) >
    requires (!std::is_member_pointer_v<decltype(Pointer)> )
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
    concept MemberPtrSpec = requires {

                                requires T::isMember;
                                {
                                    T::key
                                    } -> std::convertible_to<std::string>;
                                {
                                    T::ptr
                                    } -> std::convertible_to<typename T::ptrType>;
                                
                            };

};

#endif