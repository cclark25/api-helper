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

#define RegisterType(type, ...)             \
    template <>                             \
    struct APICore::TypeLookup<type>        \
    {                                       \
        using registeredType = __VA_ARGS__; \
        static bool isDefined;              \
    };                                      \
    bool APICore::TypeLookup<type>::isDefined = true

}

#endif