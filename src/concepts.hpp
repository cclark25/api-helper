#ifndef LUA_BINDING_CONCEPTS
#define LUA_BINDING_CONCEPTS

#include <type_traits>
#include <string>
#include <future>

namespace APICore
{
    template <class T>
    concept ReferenceTypeConcept = requires(T tval) {
                                  {
                                      *tval
                                  };

                                  requires !std::is_function_v<T>;
                              };
    
    // template<class T>
    // concept FutureTypeConcept = requires(T val) {
    //     requires std::is_same_v<std::future<decltype(val.get())>, T>;
    // };

    template <class FunctionType>
    concept ClassTypeConcept = requires {
                     requires std::is_class_v<FunctionType>;
                     requires !ReferenceTypeConcept<FunctionType>; 
                     requires !std::is_same_v<FunctionType, std::string>;
                    //  requires !FutureTypeConcept<FunctionType>;
                     requires !std::is_function_v<FunctionType>;
                     
    };

    template<ReferenceTypeConcept T>
    struct dereference {
        using type = std::remove_reference_t<decltype(*T())>;
    };
}

#endif