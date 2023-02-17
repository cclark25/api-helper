#pragma once

#include <string>

namespace APICore
{
    template <typename T>
    struct TypeLookup
    {
        struct registeredType
        {
            using inheritedFrom = void;
            using type = T;
            static const bool isClass = false;
            static std::string name;
            static std::string description;
            static void* ptr;
        };
        static bool isDefined;
    };
    template <typename T>
    bool TypeLookup<T>::isDefined = false;
    template <typename T>
    std::string TypeLookup<T>::registeredType::name = "STANDIN_NAME";
    template <typename T>
    std::string TypeLookup<T>::registeredType::description = "STANDIN_DESCRIPTION";
    template <typename T>
    void* TypeLookup<T>::registeredType::ptr = nullptr;

#define RegisterType(type, ...)             \
    template <>                             \
    struct APICore::TypeLookup<type>        \
    {                                       \
        using registeredType = __VA_ARGS__; \
        static bool isDefined;              \
    };                                      \
    bool APICore::TypeLookup<type>::isDefined = true

}
