#ifndef TYPE_BINDING_LOOKUP
#define TYPE_BINDING_LOOKUP

namespace APICore
{
    template <typename T>
    struct TypeLookup
    {
        struct registeredType
        {
            static std::string name;
            static std::string description;
        };
        static bool isDefined;
    };
    template <typename T>
    bool TypeLookup<T>::isDefined = false;
    template <typename T>
    std::string TypeLookup<T>::registeredType::name = "STANDIN_NAME";
    template <typename T>
    std::string TypeLookup<T>::registeredType::description = "STANDIN_DESCRIPTION";

    template <auto V>
    struct ValueTypeLookup : public TypeLookup<void>
    {
    };

    template <typename T, T V>
    struct ValueTypeLookup<V> : public TypeLookup<T>
    {
    };

#define RegisterType(type, ...)             \
    template <>                             \
    struct APICore::TypeLookup<type>        \
    {                                       \
        using registeredType = __VA_ARGS__; \
        static bool isDefined;              \
    };                                      \
    bool APICore::TypeLookup<type>::isDefined = true

#define RegisterValueType(value, ...)             \
    template <>                             \
    struct APICore::ValueTypeLookup<value>        \
    {                                       \
        using registeredType = __VA_ARGS__; \
        static bool isDefined;              \
    };                                      \
    bool APICore::ValueTypeLookup<value>::isDefined = true

}

#endif