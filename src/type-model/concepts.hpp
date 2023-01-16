#ifndef LUA_BINDING_CONCEPTS
#define LUA_BINDING_CONCEPTS

#include <type_traits>
#include <string>

namespace APICore
{
    template <class T>
    concept ReferenceTypeConcept = requires(T tval) {
                                  {
                                      *tval
                                  };
                              };
    
    template <class FunctionType>
    concept ClassTypeConcept = requires {
                     requires std::is_class_v<FunctionType>;
                     requires !ReferenceTypeConcept<FunctionType>; 
                     requires !std::is_same_v<FunctionType, std::string>;
    };

    template<ReferenceTypeConcept T>
    struct dereference {
        using type = std::remove_reference_t<decltype(*T())>;
    };
}

#endif