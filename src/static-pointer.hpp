#ifndef STATIC_POINTER
#define STATIC_POINTER

#include "./string-literal.hpp"
#include <type_traits>
#include <string>

namespace APICore {

    template<StringLiteral Key, auto Pointer, StringLiteral Description = "">
    struct Static {
        using type = void;
        using ptrType = void*;
        static std::string key;
        static std::string description;
        static ptrType ptr;

        static bool isConstructor;

        static const bool isMember = false;
        static const bool isCustomMember = false;
    };
    template<StringLiteral Key, auto Pointer, StringLiteral Description>
    std::string Static<Key, Pointer, Description>::key = Key.value;
    template<StringLiteral Key, auto Pointer, StringLiteral Description>
    std::string Static<Key, Pointer, Description>::description = Description.value;
    template<StringLiteral Key, auto Pointer, StringLiteral Description>
    typename Static<Key, Pointer, Description>::ptrType Static<Key, Pointer, Description>::ptr = nullptr;

    template <StringLiteral Key, auto Pointer, StringLiteral Description>
    bool Static<Key, Pointer, Description>::isConstructor = false;

    template<StringLiteral Key, typename T, T* Pointer, StringLiteral Description>
    struct Static<Key, Pointer, Description> {
        using type = T;
        using ptrType = type*;
        static std::string key;
        static std::string description;
        static ptrType ptr;
        static bool isConstructor;
        static const bool isMember = false;
        static const bool isCustomMember = false;
    };
    template<StringLiteral Key, typename T, T* Pointer, StringLiteral Description>
    std::string Static<Key, Pointer, Description>::key = Key.value;
    template<StringLiteral Key, typename T, T* Pointer, StringLiteral Description>
    std::string Static<Key, Pointer, Description>::description = Description.value;
    template<StringLiteral Key, typename T, T* Pointer, StringLiteral Description>
    typename Static<Key, Pointer, Description>::ptrType Static<Key, Pointer, Description>::ptr = Pointer;
    template<StringLiteral Key, typename T, T* Pointer, StringLiteral Description>
    bool Static<Key, Pointer, Description>::isConstructor = false;

    template<typename T>
    concept StaticPtrSpec = requires (T::type typeVal, T::ptrType ptrVal) {
       { T::key } -> std::convertible_to<std::string>;
       { T::ptr } -> std::convertible_to<typename T::ptrType>;
       { T::ptr } -> std::convertible_to<typename T::type*>;
       requires !T::isMember ;
    };
};

#endif