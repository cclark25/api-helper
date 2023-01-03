#ifndef LUA_BINDING_CONCEPTS
#define LUA_BINDING_CONCEPTS

#include <type_traits>

namespace APICore
{
    template <class T>
    concept ReferenceForLua = requires(T tval) {
                                  {
                                      *tval
                                  };
                              };
    
    template <class ClassType>
    concept ClassForLua = requires {
                     requires std::is_class_v<ClassType>;
                     requires !ReferenceForLua<ClassType>; 
    };
}

#endif