#ifndef LUA_BINDING_CONCEPTS
#define LUA_BINDING_CONCEPTS

#include <type_traits>
#include <string>
#include <functional>

namespace APICore
{
    template <class T>
    concept ReferenceTypeConcept = requires(T tval) {
                                  {
                                      *tval
                                  };

                                  requires !std::is_function_v<T>;
                                
                              };
    

    template <class FunctionType>
    concept ClassTypeConcept = requires {
                     requires std::is_class_v<FunctionType>;
                     requires !ReferenceTypeConcept<FunctionType>; 
                     requires !std::is_same_v<FunctionType, std::string>;
                     requires !std::is_function_v<FunctionType>;
                     
    };

    template<ReferenceTypeConcept T>
    struct dereference {
        using type = std::remove_reference_t<decltype(*T())>;
    };

    template<class T>
    struct IsFunctionObject {
        static const bool value = false;
    };
    // template<class T>
    // bool IsFunctionObject<T>::value = false;

    template<class R, class... P>
    struct IsFunctionObject<std::function<R(P...)>> {
        static const bool value = true;
    };
    // template<class R, class... P>
    // bool IsFunctionObject<std::function<R(P...)>>::value = true;

}

#endif