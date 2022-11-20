#ifndef MEMBER_POINTER
#define MEMBER_POINTER

#include "./string-literal.hpp"
#include <type_traits>
#include <string>

namespace APICore {
    template<auto Pointer>
    struct MemberPtr {
        using classType = void;
        using type = void;
        using ptrType = void*;
        static ptrType ptr;
    };
    template<auto Pointer>
    MemberPtr<Pointer>::ptrType MemberPtr<Pointer>::ptr = nullptr;

    template<class ClassType, typename T, T ClassType::* Pointer>
    struct MemberPtr<Pointer> {
        using classType = ClassType;
        using type = T;
        using ptrType = type classType::*;
        static ptrType ptr;
    };
    template<class ClassType, typename T, T ClassType::* Pointer>
    MemberPtr<Pointer>::ptrType MemberPtr<Pointer>::ptr = Pointer; 

    template<StringLiteral Key, auto Pointer>
    struct Member : public MemberPtr<Pointer> {
        static std::string key;
    };
    template<StringLiteral Key, auto Pointer>
    std::string Member<Key, Pointer>::key = Key.value;


    template<typename T>
    concept MemberPtrSpec = requires (T::type typeVal, T::classType classVal, T::ptrType ptrVal) {
       { T::key } -> std::convertible_to<std::string>;
       { T::ptr } -> std::convertible_to<typename T::ptrType>;
       { T::ptr } -> std::convertible_to<typename T::type T::classType::*>;
    };
};

#endif