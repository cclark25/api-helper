#pragma once

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

        static const bool isConstructor = false;

        static const bool isMember = false;
        static const bool isCustomMember = false;
    };
    template<StringLiteral Key, auto Pointer, StringLiteral Description>
    std::string Static<Key, Pointer, Description>::key = Key.value;
    template<StringLiteral Key, auto Pointer, StringLiteral Description>
    std::string Static<Key, Pointer, Description>::description = Description.value;
    template<StringLiteral Key, auto Pointer, StringLiteral Description>
    typename Static<Key, Pointer, Description>::ptrType Static<Key, Pointer, Description>::ptr = nullptr;

    template<StringLiteral Key, typename T, T* Pointer, StringLiteral Description>
    struct Static<Key, Pointer, Description> {
        using type = T;
        using ptrType = type*;
        static std::string key;
        static std::string description;
        static ptrType ptr;
        static const bool isConstructor = false;
        static const bool isMember = false;
        static bool isCustomMember ;
    };
    template<StringLiteral Key, typename T, T* Pointer, StringLiteral Description>
    std::string Static<Key, Pointer, Description>::key = Key.value;
    template<StringLiteral Key, typename T, T* Pointer, StringLiteral Description>
    std::string Static<Key, Pointer, Description>::description = Description.value;
    template<StringLiteral Key, typename T, T* Pointer, StringLiteral Description>
    typename Static<Key, Pointer, Description>::ptrType Static<Key, Pointer, Description>::ptr = Pointer;
    
    template<StringLiteral Key, typename T, T* Pointer, StringLiteral Description>
    bool Static<Key, Pointer, Description>::isCustomMember = false;

    
};
