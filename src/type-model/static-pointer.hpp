#ifndef STATIC_POINTER
#define STATIC_POINTER

#include "./string-literal.hpp"
#include <type_traits>
#include <string>

namespace APICore {

    template<StringLiteral Key, auto Pointer>
    struct Static {
        using type = void;
        using ptrType = void*;
        static std::string key;
        static ptrType ptr;
    };
    template<StringLiteral Key, auto Pointer>
    std::string Static<Key, Pointer>::key = Key.value;
    template<StringLiteral Key, auto Pointer>
    typename Static<Key, Pointer>::ptrType Static<Key, Pointer>::ptr = nullptr;

    template<StringLiteral Key, typename T, T* Pointer>
    struct Static<Key, Pointer> {
        using type = T;
        using ptrType = type*;
        static std::string key;
        static ptrType ptr;
    };
    template<StringLiteral Key, typename T, T* Pointer>
    std::string Static<Key, Pointer>::key = Key.value;
    template<StringLiteral Key, typename T, T* Pointer>
    typename Static<Key, Pointer>::ptrType Static<Key, Pointer>::ptr = Pointer;

    template<typename T>
    concept StaticPtrSpec = requires (T::type typeVal, T::ptrType ptrVal) {
       { T::key } -> std::convertible_to<std::string>;
       { T::ptr } -> std::convertible_to<typename T::ptrType>;
       { T::ptr } -> std::convertible_to<typename T::type*>;
    };
};

#endif