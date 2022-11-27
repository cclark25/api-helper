#ifndef TYPE_BINDING_LOOKUP
#define TYPE_BINDING_LOOKUP

namespace APICore {
    template<typename T>
    struct TypeLookup {
        struct registeredType {
            static std::string name;
            static std::string d;
        };
        static bool isDefined;
    };
    template<typename T>
    bool TypeLookup<T>::isDefined = false;

    #define RegisterType(type, ...) template<> struct APICore::TypeLookup<type>  { using registeredType = __VA_ARGS__; static bool isDefined; }; \
    bool APICore::TypeLookup<type>::isDefined = true

    template<> 
    struct TypeLookup<double>  { 
        using registeredType = double;
        static bool isDefined; 
    }; 
    bool APICore::TypeLookup<double>::isDefined = true;
}

#endif